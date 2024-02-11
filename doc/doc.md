# Documentation

Documentation covers following in detail:

- A sample program structure
- Different components involved
- Randomness Used
- Defining concepts used

## A sample program structure

README covers a sample program. A very simple program may have following structure:

1. Define execution context
2. Define the camera configuration
3. Define the world
4. Define the image
5. Run the algorithm

We would cover these things in detail in upcoming sections.

## Different components involved

### Execution Context

Speaking on a high level, ray tracer needs to run some algorithm to generate
the image. Now we can run this algorithm on different execution context like
- Single Thread
- Thread Pool
- GPU
- On some remote machine
based on our usecase.

Execution context determines where the rendering algorithm runs.
In code we call the reference to execution context as **Scheduler**.

### Camera && Camera Orientation

We look the world through eyes. As our eyes move or change its orientation,
how the world looks changes. In terms of ray tracing camera acts as our eyes and
camera orientation as orientation of our eyes (e.g., we are facing forward but
our eye is looking upwards).

There is subtle difference between how both works. In case of eye, we capture
the all the rays coming to us. However, this is computationally
very expensive. In case of camera, we send the rays in opposite direction from
camera and observe it after iteraction with world.

### World

World contains different objects. Rays from camera hit these objects and
deflects and change color. World itself can be treated as object. In terms
of code we call objects as **SceneObject**. When ray hits the scene object, it
hits some part of hit. That part that got hit is called **HitObject**.

### Image

Image can be treated as matrix of colors (pixels). Its the 2d representation of
how we see the 3d world. This is the output of rendering algorithm.

### Rendering algorithm

Rendering algorithm takes execution context, camera, camera orientation, world
and image (for filling output colors) as argument and runs the algorithm on
execution context and fills the image with how camera sees the world.

## Randomness used

Rendering algorithm and its different components have some sort of randomness
involved. This randomness is there to model the irregularity in nature.

For example let's say there is a sphere, its possible that its material is
rough and light does not reflect with same angle of incidence with normal
of sphere. Modelling exact roughness is impractical and computationally very
expensive. Hence randomness is used to model these kind of irregularities.

There are other examples of irregularity too, and there randomness is used.
Hence ray interaction algorithms have an additional random generator argument
too.

## Defining concepts used

Now we have a higher level picture of how mraylib works. We can go through
different concepts in detail and look at library users' perspective how they
can use/extend the same.

We would use C++20 concepts signature extensively.

### Generator
Generator has a free function gen defined that generates a value between
[min, max).

```cpp
template <typename generator_t, typename ValueType>
concept Generator =
    std::totally_ordered<ValueType> &&
    requires(generator_t &gen, ValueType const &min, ValueType const &max) {
      // Postcondition: generates a value in range [min, max)
      { gen(min, max) } -> std::same_as<ValueType>;
    };
```

A **DoubleGenerator** is a Generator<double>.

### Scheduler

Scheduler is handle to execution context on which we can schedule any task.

```cpp
template <typename scheduler_t>
concept Scheduler =
    stdexec::scheduler<scheduler_t> &&
    requires(scheduler_t scheduler, unsigned long random_seed) {
      // Postcondition: DoubleGenerator should be parallel execution safe.
      {
        random_generator(scheduler, random_seed)
      } -> DoubleGenerator;
    };
```

A Scheduler is P2300's scheduler with an additional requirement that it
has a free function `random_generator` defined that takes scheduler and
a seed that returns a parallel random double generator.
That means if Generator `gen` is returned from random_generator and
scheduler can execute multiple tasks concurrently, it is assumed that
mutable reference of gen can be passed to those concurrent tasks and it would
not lead to any data race.

### Image

Because our rendering algorithm is built with parallelism in mind we mostly
deal with **RandomAccessImage** and **OutputRandomAccessImage**.

```cpp
template <typename Image>
concept RandomAccessImage =
    requires(Image const &img, int x, int y, color_t const &color) {
      { width(img) } -> std::same_as<int>;
      { height(img) } -> std::same_as<int>;
      { pixel_at(img, x, y) } -> std::convertible_to<color_t>;
    };

template <typename Image>
concept OutputRandomAccessImage =
    RandomAccessImage<Image> &&
    requires(Image &img_mut, int x, int y, color_t const &color) {
      { set_pixel_at(img_mut, x, y, color) };
    };
```

RandomAccessImage provides us to query width, height and pixel at any coord
of an image. x is assumed to be aligned with width. OutputRandomAccessImage
provides additional free function to set the pixel at a coord.

Library has an inbuilt `in_memory_image` type that represents the whole image
in memory. It models OutputRandomAccessImage.

### Camera

`camera_t` is a pure data structure. It has 3 fields:

- focus_distance: this determines the distance between camera and its viewport
- vertical_fov: vertical field of view angle
- defocus_angle: how much light goes through camera hole (to control focus).

### Camera Orientation

`camera_orientation_t` is also a pure data structure. It has 3 fields:

- look_from: this determines the position of camera
- look_at: this determines the direction where camera is looking at
- up_dir: this determines how camera is tilted on axis of (look_at - look_from)

### interval_t

`interval_t` is a pure data structure defining values between [min, max].

### scale_2d

`scale_2d_t` is a pure data structure containing x_scale and y_scale. Both
lies between [0, 1]. This determines how 3d object looks in 2d.

### Shape

To define the world, we need objects. Many times objects are composed of
mathematical shapes. We have a concept of Shape.

```cpp
template <typename shape_t>
// Precondition:
//   - p should lie on surface of shape
concept Shape = requires(shape_t const &shape, ray_t const &r, point3 const &p,
                         interval_t const &i) {
  // Postconditon:
  //   - normal points to outside the object
  { normal_at(shape, p) } -> std::same_as<direction_t>;
  { scaling_2d_at(shape, p) } -> std::same_as<scale_2d_t>;
  { ray_hit_distance(shape, r, i) } -> std::same_as<std::optional<double>>;
};
```

For any shape we should be able to
- get normal at any point on its surface.
- get 2d scaling of any point.
- get at how much distance a ray hits the shape (first time) with possible
  distance in interval i.

Library users most probably want to use some non inbuilt complex shape. So,
let's see how to create a new shape from scratch.

Sphere has really simple attributes:
```cpp
struct sphere {
  double radius;
  point3 center;
};
```

Let's define normal:
```cpp
constexpr direction_t normal_at(sphere const &s, point3 const &p) {
  return p - s.center;
}
```

Let's define 2d scaling:
```cpp
constexpr scale_2d_t scaling_2d_at(sphere const &s, point3 const &p) {
  auto const normal = normal_at(s, p).val();
  constexpr static double pi = M_PI;
  auto theta = acos(-normal.y);
  auto phi = atan2(-normal.z, normal.x) + pi;
  return {phi / (2 * pi), theta / pi};
}
```

And some complex maths for hit distance of ray:
```cpp
constexpr std::optional<double>
ray_hit_distance(sphere const &obj, ray_t const &r, interval_t const &t_range) {
  auto oc = r.origin - obj.center;
  auto a = r.direction.val().length_square();
  auto half_b = dot(oc, r.direction.val());
  auto c = oc.length_square() - obj.radius * obj.radius;
  auto discriminant = half_b * half_b - a * c;
  auto discriminant_sqrt = std::sqrt(discriminant);
  if (discriminant >= 0) {
    auto t1 = (-half_b - discriminant_sqrt) / a;
    if (t_range.surrounds(t1))
      return t1;
    auto t2 = (-half_b + discriminant_sqrt) / a;
    if (t_range.surrounds(t2))
      return t2;
  }
  return std::nullopt;
}
```

And if we want to support bvh acceleration then:
```cpp
constexpr bound_t get_bounds(sphere const &sphere) {
  return {
      .x_range = interval_t{sphere.center.x - sphere.radius,
                            sphere.center.x + sphere.radius},
      .y_range = interval_t{sphere.center.y - sphere.radius,
                            sphere.center.y + sphere.radius},
      .z_range = interval_t{sphere.center.z - sphere.radius,
                            sphere.center.z + sphere.radius},
  };
}
```

We would discuss about bvh and bound_t later.

Currently we have sphere and quad inbuilt shape in library.

### Materials and Textures

An object may have a shape, but only shape doesn't determine how ray
interact with object. If object is rough, then ray interact in irregular
fashion, but if object is smooth, ray interacts according to law of reflection.
If object is transparent, then we have refraction.

So, material of object determines how ray interacts with object. Mainly
it determines:
- How rays scatter after hitting
- If material emit some rays (like sun)
- What would be the impact of color of resulting ray.

While material describes the interaction of rays with object. Texture describes
how material looks. Let's say its a solid red color material with rough surface.
So, this information of solid red color is what texture describes.

A material can be a light scatterer or a light emitter.

```cpp
template <typename T, typename Generator>
concept LightScatterer =
    DoubleGenerator<Generator> &&
    requires(T const &material, scattering_context const &ctx,
             generator_view<Generator> rand) {
      {
        scatter(material, ctx, rand)
      } -> std::same_as<std::optional<scatter_info_t>>;
    };

template <typename T, typename Generator>
concept LightEmitter = DoubleGenerator<Generator> &&
                       requires(T const &light, emission_context const ctx,
                                generator_view<Generator> rand) {
                         {
                           emit(light, ctx, rand)
                         } -> std::same_as<std::optional<emit_info_t>>;
                       };
```

Here we see that scatter and emit function takes an additional double random
generator parameter as argument. They can use this argument for any sort
of randomness required.

Similarily, we have Texture concept:
```cpp
template <typename T, typename Generator>
concept Texture =
    DoubleGenerator<Generator> &&
    requires(T const &texture, scale_2d_t const &coord, point3 const &hit_point,
             generator_view<Generator> rand) {
      {
        texture_color(texture, coord, hit_point, rand)
      } -> std::same_as<color_t>;
    }
```

Let's see how we can write a custom simple texture and using that how can we
write a custom simple material.

Let's start with a texture, that is simply of same color everywhere. We
would call it solid_color_texture.

```cpp
struct solid_color_texture {
  color_t color;
};
```
It literally just needs a color.

Now let's define a function to support texture_color of our concept Texture.

```cpp
template <DoubleGenerator Generator>
constexpr color_t texture_color(solid_color_texture const &texture,
                                scale_2d_t const &, point3 const &,
                                generator_view<Generator>) {
  return texture.color;
}
```

scale_2d_t is a non-obvious argument to texture_color. This is needed when
texture wants to pick color from some 2d matrix like an image. So, we need
this mapping of 3d view to 2d view and vice-versa.

Currently we have following textures inbuilt:
- solid_color
- perlin_texture
- image_texture
- checker_texture

Now, let's define a material that is rough is nature. This is kind of
similar material like a wall. It looks the same way no matter from where we
look from. We call this lambertian material. It only scatters light not
emit any light, thus its a LightScatterer.

```cpp
template <typename Texture> struct lambertian_t {
  Texture texture;
};
```

Now let's define the scatter function for LightScatterer.

```cpp
constexpr std::optional<scatter_info_t>
lambertian_scatter(color_t const &material_color, ray_t const &in_ray,
                   point3 hit_point, direction_t normal,
                   direction_t const &random_dir) {
  normal = normal_dir(normal, in_ray.direction);
  auto scatter_dir = normal.val() + random_dir.val();
  if (near_zero(scatter_dir))
    scatter_dir = normal.val();
  auto scattered_ray = ray_t{
      .origin = hit_point,
      .direction = scatter_dir,
  };
  auto attenuation = material_color;
  return scatter_info_t{
      .scattered_ray = scattered_ray,
      .attenuated_color = attenuation,
  };
}

template <DoubleGenerator Generator, Texture<Generator> texture_t>
constexpr auto scatter(lambertian_t<texture_t> const &material,
                       scattering_context const &ctx,
                       generator_view<Generator> rand) {
  auto color =
      texture_color(material.texture, ctx.scaling_2d, ctx.hit_point, rand);
  return lambertian_scatter(color, ctx.ray, ctx.hit_point, ctx.normal,
                            direction_generator{}(rand));
}
```

Here if light scatters in a random fashion, it resembles the apperance of
a wall like material.

Currently we have following materials inbuilt:
- metal
- lambertian
- dielectric
- diffuse_light
