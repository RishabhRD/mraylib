#include "aspect_ratio.hpp"
#include "camera.hpp"
#include "camera_orientation.hpp"
#include "color.hpp"
#include "image/ppm/ppm_utils.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "scene.hpp"
#include "scene_viewer.hpp"
#include "vector.hpp"
#include "viewport.hpp"
#include "viewport_utils.hpp"
#include <iostream>

constexpr auto make_scene_viewer(int scene_width, int scene_height) {
  double focal_length = 1.0;
  double viewport_height = 2.0;
  double viewport_width =
      mrl::viewport_width({scene_width, scene_height}, viewport_height);
  mrl::point3 camera_poisition{0, 0, 0};
  mrl::vec3 camera_direction{0, 0, 1};

  mrl::camera_t camera{
      .viewport =
          {
              .width = viewport_width,
              .height = viewport_height,
          },
      .focal_length = focal_length,
  };
  mrl::camera_orientation_t orientation{
      .position = camera_poisition,
      .direction = camera_direction,
  };
  return mrl::scene_viewer_t{
      .camera = camera,
      .camera_orientation = orientation,
  };
}

mrl::color_t ray_color(mrl::ray_t const &) { return {0, 0, 0}; }

int main() {
  mrl::aspect_ratio_t ratio{16, 9};
  auto scene_width = 400;
  auto scene_height = mrl::scene_height(ratio, scene_width);
  std::cerr << scene_height << std::endl;

  mrl::scene_viewer_t scene_viewer =
      make_scene_viewer(scene_width, scene_height);

  auto [u_dir, v_dir] =
      mrl::viewport_direction(scene_viewer.camera_orientation);
  auto viewport_u = u_dir * scene_viewer.camera.viewport.width;
  auto viewport_v = u_dir * scene_viewer.camera.viewport.height;

  auto pixel_delta_u = viewport_u / scene_width;
  auto pixel_delta_v = viewport_v / scene_height;

  auto viewport_top_left = mrl::viewport_topleft(
      viewport_u, viewport_v, scene_viewer.camera_orientation,
      scene_viewer.camera.focal_length);

  auto pixel00_loc = viewport_top_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  std::cout << "P3\n" << scene_width << ' ' << scene_height << "\n255\n";

  for (int j = 0; j < scene_height; ++j) {
    for (int i = 0; i < scene_width; ++i) {
      auto pixel_center = pixel00_loc + i * pixel_delta_u + j * pixel_delta_v;
      auto camera_center = scene_viewer.camera_orientation.position;
      auto ray_direction =
          pixel_center - scene_viewer.camera_orientation.position;
      mrl::ray_t r{.origin = camera_center, .direction = ray_direction};
      auto color = ray_color(r);
      std::cout << mrl::convert_to_ppm_str(color);
    }
  }
}
