#include "color.hpp"
#include "mraylib.hpp"
#include "textures/solid_color.hpp"
#include <fstream>
#include <vector>

using namespace mrl;

int main() {
  // Configure Execution Context
  auto const num_threads = std::thread::hardware_concurrency();
  auto th_pool = thread_pool{num_threads};
  auto sch = th_pool.get_scheduler();

  auto cur_time = static_cast<unsigned long>(
      std::chrono::system_clock::now().time_since_epoch().count());

  // Configure the camera
  camera_t camera{
      .focus_distance = 10.0,
      .vertical_fov = degrees(20),
      .defocus_angle = degrees(0),
  };
  camera_orientation_t camera_orientation{
      .look_from = point3{13, 2, 3},
      .look_at = point3{0, 0, 0},
      .up_dir = direction_t{0, 1, 0},
  };

  // Define the world
  perlin_texture texture{solid_color_texture{0.78, 0.4, 0.1},
                         perlin_noise{cur_time}, 4};
  perlin_texture small_texture{solid_color_texture{from_rgb(14, 90, 138)},
                               perlin_noise{cur_time}, 4};
  lambertian_t material{texture};
  lambertian_t small_material{std::move(small_texture)};
  shape_object big_sphere{sphere{1000, point3{0, -1000, 0}}, material};
  shape_object small_sphere{sphere{2, point3{0, 2, 0}},
                            std::move(small_material)};

  using any_object = any_object_t<decltype(sch)>;
  std::vector<any_object> world{big_sphere, std::move(small_sphere)};
  // Acceleration structure
  bvh_t<any_object> bvh{std::move(world)};

  auto background = color_t{0.7, 0.8, 1.0};

  // Define the image
  aspect_ratio_t ratio{16, 9};
  auto img_width = 600;
  auto img_height = image_height(ratio, img_width);
  in_memory_image img{img_width, img_height};
  auto path = std::getenv("HOME") + std::string{"/perlin_texture.ppm"};
  std::ofstream os(path, std::ios::out);

  // Actually run the algorithm
  img_renderer_t renderer(camera, camera_orientation, background, sch,
                          cur_time);
  stdexec::sync_wait(renderer.render(bvh, img));
  write_ppm_img(os, img);
}
