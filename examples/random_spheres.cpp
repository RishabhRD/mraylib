#include "mraylib.hpp"
#include <fstream>
#include <vector>

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

  // Define the world
  using any_object = any_object_t<decltype(sch)>;
  auto cur_time = static_cast<unsigned long>(
      std::chrono::system_clock::now().time_since_epoch().count());
  auto rand = random_generator(sch, cur_time);
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

  for (int a = -11; a < 11; ++a) {
    for (int b = -11; b < 11; ++b) {
      auto choose_mat = rand(0.0, 1.0);
      auto center =
          point3{a + 0.9 * rand(0.0, 1.0), 0.2, b * 0.9 * rand(0.0, 1.0)};
      if (choose_mat < 0.8) {
        auto color = color_t{rand(0.0, 1.0), rand(0.0, 1.0), rand(0.0, 1.0)} *
                     color_t{rand(0.0, 1.0), rand(0.0, 1.0), rand(0.0, 1.0)};
        world.push_back(shape_object{sphere{0.2, center}, lambertian_t{color}});
      } else if (choose_mat < 0.95) {
        auto color = color_t{rand(0.0, 1.0), rand(0.0, 1.0), rand(0.0, 1.0)};
        auto fuzz = rand(0.5, 1.0);
        world.push_back(
            shape_object{sphere{0.2, center}, fuzzy_metal_t{color, fuzz}});

      } else {
        world.push_back(shape_object{sphere{0.2, center}, dielectric{1.5}});
      }
    }
  }
  auto background = color_t{0.7, 0.8, 1.0};

  // Acceleration structure
  bvh_t<any_object> bvh{std::move(world)};

  // Define the image
  aspect_ratio_t ratio{16, 9};
  auto img_width = 1000;
  auto img_height = image_height(ratio, img_width);
  in_memory_image img{img_width, img_height};
  auto path = std::getenv("HOME") + std::string{"/random_spheres.ppm"};
  std::ofstream os(path, std::ios::out);

  // Actually run the algorithm
  img_renderer_t renderer(camera, camera_orientation, background, sch,
                          cur_time);
  stdexec::sync_wait(renderer.render(bvh, img));
  write_ppm_img(os, img);
}
