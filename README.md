# mraylib

mraylib is a C++23 pure algorithmic ray tracing library.

mraylib focuses on following:
- Design by contract \[WIP\]
- Independent of Execution Context (use Senders/Receivers)
- Pure Algorithmic

<p align="center">
  <img src='https://github.com/RishabhRD/mraylib/assets/26287448/f040787d-c36a-4731-8fb3-39857ec25a53' width='500'>
</p>

### Design by contract \[WIP\]
I started mraylib as an exercise of trying to write something real with what I
felt is good code. Any feedbacks or contribution in this direction is highly
appreciated.

### Independent of Execution Context
I have seen many ray tracers that written for thread pool or single thread or
cuda. However, I wanted this ray tracer to be independent of execution context.
This needed a good abstraction over exeuctors. [P2300](https://wg21.link/P2300)
is a good proposal for the same. mraylib depends on [stdexec](https://github.com/NVIDIA/stdexec)
(an implementation of P2300) algorithms for its execution.

Library assumes stdexec to be present while compilation.

### Pure Algorithmic
mraylib algorithms depends on concepts (requirements) instead of depending
on concrete types. For example render algorithm depends on `OutputRandomAccessImage`
concept instead of depending on any specific image implementation.
A library user can use any type that satisfies these concepts for algorithms.

## A sample Program

```cpp
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
  auto img_width = 1000;
  auto img_height = image_height(ratio, img_width);
  in_memory_image img{img_width, img_height};
  auto path = std::getenv("HOME") + std::string{"/img.ppm"};
  std::ofstream os(path, std::ios::out);

  // Actually run the algorithm
  auto background = color_t{0.7, 0.8, 1.0};
  auto cur_time = static_cast<unsigned long>(
      std::chrono::system_clock::now().time_since_epoch().count());
  img_renderer_t renderer(camera, camera_orientation, background, sch,
                          cur_time);
  stdexec::sync_wait(renderer.render(bvh, img));
  write_ppm_img(os, img);
```
