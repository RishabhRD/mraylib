#pragma once

#include "camera.hpp"
#include "camera_orientation.hpp"
#include "color.hpp"
#include "image/concepts.hpp"
#include "ray.hpp"
#include "viewport_utils.hpp"

namespace mrl {

constexpr color_t ray_color(ray_t const &ray) {
  auto dir = ray.direction.val();
  auto a = 0.5 * (dir.x + 1.0);
  return (1.0 - a) * color_t{1.0, 1.0, 1.0} + a * color_t{0.5, 0.7, 1.0};
}

template <RandomAccessImage Image>
constexpr void trace(camera_t const &camera,
                     camera_orientation_t const &orientation,
                     scene_t const &scene, Image &img) {
  auto [u_dir, v_dir] = viewport_direction(orientation);
  auto viewport_u = u_dir * camera.viewport.width;
  auto viewport_v = u_dir * camera.viewport.height;

  auto pixel_delta_u = viewport_u / scene.width;
  auto pixel_delta_v = viewport_v / scene.height;

  auto viewport_top_left = viewport_topleft(viewport_u, viewport_v, orientation,
                                            camera.focal_length);

  auto pixel00_loc = viewport_top_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  for (int j = 0; j < scene.height; ++j) {
    for (int i = 0; i < scene.width; ++i) {
      auto pixel_center = pixel00_loc + i * pixel_delta_u + j * pixel_delta_v;
      auto camera_center = orientation.position;
      ray_t r{
          .origin = camera_center,
          .direction = pixel_center - orientation.position,
      };
      img.at(j, i) = ray_color(r);
    }
  }
}
} // namespace mrl
