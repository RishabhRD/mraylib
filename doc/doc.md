# Documentation

Documentation covers following in detail:

- A sample program structure
- Different components involved
- Randomness Used
- Defining concepts used
- A full program

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

### Camera and Camera Orientation

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

### SceneObject and HitObject

SceneObject is what ray actually interacts with. Renderer thinks SceneObject
as world. When ray hits SceneObject, it hits some part of it. The part got
hit is called HitObject.

SceneObject encapsulates all the details of shapes, materials, textures, etc
and present a higher level picture to rendering algorithm. Rendering algorithm
doesn't know anything about internal details like shapes, materials, textures,
etc.

Let's look at SceneObject:
```cpp
template <typename Object>
concept Hittable =
    requires(Object const &obj, ray_t const &ray, interval_t const &interval) {
      typename hit_object_t<Object>;
      {
        hit(obj, ray, interval)
      } -> std::same_as<std::optional<hit_info_t<hit_object_t<<Object>>>>;
    };

template <typename Object>
concept SceneObject = Hittable<Object>;
```

SceneObject only cares about hit:
- What type you get, when ray hits the object. (hit_object_t<Object>)
- And hit_info you get if ray hits the object in a certain distance inteval.

If ray doesn't hit, hit should return nullopt.

Defining hit_object_t is simple. Let's say you have a SceneObject of type
Obj, then you can do any one of 2 things:

```cpp
struct Obj{
  using hit_object_type = <something>;
  ... (implementation of Obj)
};
```
```cpp
struct Obj{
  ... (implementation of Obj)
};

template <>
struct hit_object<Obj>{
  using type = <something>;
};
```

Second method is really useful when you can't modify struct Obj.

hit_info_t is a simple struct to encapsulate information about the ray hit:
```cpp
template <typename HitObject> struct hit_info_t {
  using hit_object_t = HitObject;

  double hit_distance;
  hit_object_t hit_object;
};
```
It simply contains the hit_distance and the hit_object.

Let's define a really simple obvious SceneObject aka shape_object. shape_object
is made up of a shape and a material.

Let's first declare shape_object:
```cpp
template <Shape shape_t, typename material_t> struct shape_object;
```

Let's define the HitObject for the shape_object:
```cpp
template <Shape shape_t, typename material_t> struct shape_hit_object {
  shape_object<shape_t, material_t> const *obj;
};
```

And now finally define the shape_object:
```cpp
template <Shape shape_t, typename material_t> struct shape_object {
  using shape_type = shape_t;
  using material_type = material_t;
  using hit_object_type = shape_hit_object<shape_t, material_t>;

  shape_type shape;
  material_type material;

  constexpr shape_object(shape_type shape_, material_type material_)
      : shape(std::move(shape_)), material(std::move(material_)) {}
};

// Use CTAD for easy initialization
template <Shape shape_t, typename material_t>
shape_object(shape_t, material_t) -> shape_object<shape_t, material_t>;
```

For meeting expectations of ShapeObject, we need to define the hit function:
```cpp
template <Shape shape_t, typename material_t>
constexpr std::optional<hit_info_t<shape_hit_object<shape_t, material_t>>>
hit(shape_object<shape_t, material_t> const &obj, ray_t const &ray,
    interval_t const &interval) {
  auto hit_dist_opt = ray_hit_distance(obj.shape, ray, interval);
  if (!hit_dist_opt) {
    return std::nullopt;
  }
  return hit_info_t<shape_hit_object<shape_t, material_t>>{
      *hit_dist_opt, shape_hit_object<shape_t, material_t>{&obj}};
}
```

And get_bounds for supporting bvh acceleration:
```cpp
template <Shape shape, typename material_t>
constexpr bound_t get_bounds(shape_object<shape, material_t> const &obj) {
  return get_bounds(obj.shape);
}
```

Now we have ShapeObject, but shape_hit_object is currently useless. For
it being useful, it should follow HitObject concept:

```cpp
template <typename Object, typename Generator>
concept HitObject =
    DoubleGenerator<Generator> &&
    // Precondition:
    //   - p is a point on surface of obj
    requires(Object const &obj, generator_view<Generator> rand, point3 const &p,
             ray_t const &r, double hit_distance) {
      { normal_at(obj, p) } -> std::same_as<direction_t>;
      { scaling_2d_at(obj, p) } -> std::same_as<scale_2d_t>;
      {
        scattering_for(obj, r, hit_distance, rand)
      } -> std::same_as<std::optional<scatter_info_t>>;
      { emission_at(obj, p, rand) } -> std::same_as<std::optional<emit_info_t>>;
    };
```

We can do following query to HitObject:
- What is normal at any point on its surface?
- Where does any point on surface belong to 2d scaling?
- What would would scattering for ray r hitting at hit_distance?
- What would be emission at any point on the surface?

HitObject may or may not scatter or emit ray. In those cases, they can return
nullopt.

**NOTE:** It is assumed that the parent scene object is alive during these
queries.

Now, let's make our shape_hit_object to model HitObject:

```cpp
template <Shape shape_t, typename material>
constexpr auto normal_at(shape_hit_object<shape_t, material> const &o,
                         point3 const &p) {
  return normal_at(o.obj->shape, p);
}

template <Shape shape_t, typename material>
constexpr auto scaling_2d_at(shape_hit_object<shape_t, material> const &o,
                             point3 const &p) {
  return scaling_2d_at(o.obj->shape, p);
}

template <DoubleGenerator Generator, Shape shape_t, typename material_t>
constexpr std::optional<scatter_info_t>
scattering_for(shape_hit_object<shape_t, material_t> const &o, ray_t const &r,
               double hit_distance, generator_view<Generator> rand) {
  if constexpr (LightScatterer<material_t, Generator>) {
    auto const &material = o.obj->material;
    auto ctx = make_scattering_context(o, r, hit_distance);
    return scatter(material, ctx, rand);
  } else {
    return std::nullopt;
  }
}

template <DoubleGenerator Generator, Shape shape_t, typename material_t>
constexpr std::optional<emit_info_t>
emission_at(shape_hit_object<shape_t, material_t> const &o, point3 const &p,
            generator_view<Generator> rand) {
  if constexpr (LightEmitter<material_t, Generator>) {
    auto const &material = o.obj->material;
    auto ctx = make_emission_context(o, p);
    return emit(material, ctx, rand);
  } else {
    return std::nullopt;
  }
}
```

Currently we have following SceneObject inbuilt:
- shape_object
- object_ref (just a reference wrapper to an object)
- translate_object
- rotate_object
- any_object (type erased scene object)

Any std::ranges::input_range<T> where T is a SceneObject is also a SceneObject
automatically.

any_object helps to have hetrogeneous collection of scene objects.
For example:
```cpp
  auto const num_threads = std::thread::hardware_concurrency();
  auto th_pool = thread_pool{num_threads};
  auto sch = th_pool.get_scheduler();
  using any_object = any_object_t<decltype(sch)>;

  std::vector<any_object> world;

  lambertian_t mat1{color_t{0.4, 0.2, 0.1}};
  metal_t mat2(color_t{0.7, 0.6, 0.5});

  shape_object obj1{sphere{1.0, point3{0, 1, 0}}, mat1};
  shape_object obj2{sphere{1.0, point3{-4, 1, 0}}, mat2};

  // obj1 and obj2 are of different type because of being of different material
  // but can be pushed to world:
  world.push_back(obj1);
  world.push_back(obj2);
```

### bvh_t and bound_t

bvh is an acceleration data structure, that is also a SceneObject.
Using this structure, ray doesn't need to hit all objects inside it, but
only logarithmic number of those objects.

So, it converts hit from O(n) to O(log n).

bound_t is axis aligned bound. It is a cuboid that is aligned with X,Y and Z
axes (i.e., 2 faces are parallel 2 x-axis, 2 to y-axis and 2 to z-axis).

This represent a bound around object that object is encapsulated inside the
bounds.

We want to explore more on acceleration structure and any contribution towards
it is highly appreciated.

bound_t is really simple:
```cpp
struct bound_t {
  interval_t x_range;
  interval_t y_range;
  interval_t z_range;
};
```

### Sampler

Rendering algorithm actually sends multiple ray to generate a single pixel. It
averages the result of multiple rays to render that pixel. This is done to
reduce the noise and errors.

Sampler concepts is there for determining the sample points for that pixel.

It nees a sampler_args_t:
```cpp
struct sampler_args_t {
  point3 point;       // point on viewport through which ray is sent and that maps to a pixel
  vec3 pixel_delta_u; // distance between 2 pixels in x direction
  vec3 pixel_delta_v; // distance between 2 pixels in y direction
};
```

Concept sampler is as follows:

```cpp
template <typename Sampler, typename random_generator>
concept PixelSampler =
    DoubleGenerator<random_generator> &&
    requires(Sampler const &sampler, sampler_args_t const &args,
             generator_view<random_generator> gen_random) {
      // Postcondition:
      //   - Let's say returned range is rng
      //   - size(rng) >= 1
      //   - All points in rng should lie on viewport plane
      { sampler(args, gen_random) } -> std::ranges::input_range<T> && RangeValueType<T, point3>;;
    };
```

A really simple sampler is a sampler that just returns the point from which
ray was sent:
```cpp
struct identity_sampler {
  template <DoubleGenerator Generator>
  constexpr auto operator()(sampler_args_t const &args,
                            generator_view<Generator>) const {
    return std::views::single(args.point);
  }
};
```

## A full program

```cpp
#include "mraylib.hpp"
#include <fstream>
#include <vector>

// Output:
// https://github.com/RishabhRD/mraylib/assets/26287448/86d083e7-6f4d-4879-aee0-819189a8c81d

using namespace mrl;

int main() {
  // Configure Execution Context
  auto const num_threads = std::thread::hardware_concurrency();
  auto th_pool = thread_pool{num_threads};
  auto sch = th_pool.get_scheduler();

  // Configure camera (how we look into the world)
  camera_t camera{
      .focus_distance = 10.0,
      .vertical_fov = degrees(20),
      .defocus_angle = degrees(0.0),
  };
  camera_orientation_t camera_orientation{
      .look_from = {13, 2, 3},
      .look_at = point3(0, 0, 0),
      .up_dir = direction_t{0, 1, 0},
  };

  // Defining the world
  using any_object = any_object_t<decltype(sch)>;
  std::vector<any_object> world;
  dielectric mat1(1.5);
  lambertian_t mat2(color_t{0.4, 0.2, 0.1});
  metal_t mat3(color_t{0.7, 0.6, 0.5});

  auto checker = lambertian_t{
      checker_texture{0.32, solid_color_texture{from_rgb(38, 39, 41)},
                      solid_color_texture{color_t{.9, .9, .9}}}};

  world.push_back(shape_object{sphere{1.0, point3{0, 1, 0}}, mat1});
  world.push_back(shape_object{sphere{1.0, point3{-4, 1, 0}}, mat2});
  world.push_back(shape_object{sphere{1.0, point3{4, 1, 0}}, mat3});
  world.push_back(shape_object{sphere{1000.0, point3{0, -1000, 0}}, checker});
  // Acceleration structure for handling large number of objects
  bvh_t<any_object> bvh{std::move(world)};

  // Defining Image
  aspect_ratio_t ratio{16, 9};
  auto img_width = 600;
  auto img_height = image_height(ratio, img_width);
  in_memory_image img{img_width, img_height};
  auto path = std::getenv("HOME") + std::string{"/readme.ppm"};
  std::ofstream os(path, std::ios::out);

  // Actually run the algorithm
  auto background = color_t{0.7, 0.8, 1.0};
  auto cur_time = static_cast<unsigned long>(
      std::chrono::system_clock::now().time_since_epoch().count());
  img_renderer_t renderer(camera, camera_orientation, background, sch,
                          cur_time);
  stdexec::sync_wait(renderer.render(bvh, img));
  write_ppm_img(os, img);
}
```
