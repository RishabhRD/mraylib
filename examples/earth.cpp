#include "image/stb_image.hpp"
#include "mraylib.hpp"
#include "schedulers/libdispatch_queue.hpp"
#include <fstream>
#include <vector>

// Output:
// https://github.com/RishabhRD/mraylib/assets/26287448/adecba25-c86a-4e29-bb7e-f67d4e6cbbfe

using namespace mrl;

// NOTE: This example depends on third part stb library
int main() {
  // Configure Execution Context
  libdispatch_queue queue;
  auto sch = queue.get_scheduler();

  // Configure camera (how we look into the world)
  camera_t camera{
      .focus_distance = 10.0,
      .vertical_fov = degrees(20),
      .defocus_angle = degrees(0),
  };
  camera_orientation_t camera_orientation{
      .look_from = point3{0, 0, 0},
      .look_at = point3{0, 0, 15},
      .up_dir = direction_t{0, 1, 0},
  };

  // Define the world
  // NOTE: assumes data/earthmap.jpg to be present
  auto earth_img = stb_image{"data/earthmap.jpg"};
  auto earth_texture = image_texture{std::move(earth_img)};
  auto earth_surface = lambertian_t{std::move(earth_texture)};
  auto globe =
      shape_object{sphere{2, point3{0, 0, 15}}, std::move(earth_surface)};

  // NOTE: assumes data/space.jpg to be present
  auto space_img = stb_image{"data/space.jpg"};
  auto space_texture = image_texture{std::move(space_img)};
  auto space_surface = lambertian_t{std::move(space_texture)};
  auto space =
      shape_object{sphere{10, point3{0, 0, 25}}, std::move(space_surface)};

  std::vector<decltype(space)> world;
  world.push_back(std::move(globe));
  world.push_back(std::move(space));

  auto background = color_t{0.7, 0.8, 1.0};

  // Define the image
  aspect_ratio_t ratio{16, 9};
  auto img_width = 1000;
  auto img_height = image_height(ratio, img_width);
  in_memory_image img{img_width, img_height};
  auto path = std::getenv("HOME") + std::string{"/earth.ppm"};
  std::ofstream os(path, std::ios::out);

  // Actually run the algorithm
  auto cur_time = static_cast<unsigned long>(
      std::chrono::system_clock::now().time_since_epoch().count());
  img_renderer_t renderer(camera, camera_orientation, background, sch,
                          cur_time);
  stdexec::sync_wait(renderer.render(world, img));
  write_ppm_img(os, img);
}
