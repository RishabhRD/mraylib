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
