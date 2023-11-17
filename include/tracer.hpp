#pragma once

#include "camera.hpp"
#include "camera_orientation.hpp"
#include "color.hpp"
#include "image/concepts.hpp"
#include "ray.hpp"
#include "scene_objects/shapes/sphere.hpp"
#include "viewport_utils.hpp"

namespace mrl {

constexpr color_t ray_color(ray_t const &ray) {
  sphere_obj_t sphere{
      .sphere =
          {
              .radius = 0.5,
          },
      .center = {0, 0, -1},
  };
  auto t_opt = hit(sphere, ray);
  if (t_opt) {
    auto t = *t_opt;
    auto N = normal(sphere, ray.at(t)).val();
    return 0.5 * color_t(N.x + 1, N.y + 1, N.z + 1);
  }
  auto c1 = from_rgb(179, 89, 29);
  auto c2 = from_rgb(255, 255, 255);
  auto dir = ray.direction.val();
  auto a = (dir.y + 1.0) / 2;
  return (1.0 - a) * c1 + a * c2;
}

template <RandomAccessImage Image>
constexpr void trace(camera_t const &camera,
                     camera_orientation_t const &orientation,
                     scene_t const &scene, Image &img) {
  auto [u_dir, v_dir] = viewport_direction(orientation);
  auto viewport_u = u_dir * camera.viewport.width;
  auto viewport_v = v_dir * camera.viewport.height;

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
