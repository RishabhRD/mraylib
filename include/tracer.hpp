#pragma once

#include "camera.hpp"
#include "camera_orientation.hpp"
#include "color.hpp"
#include "direction.hpp"
#include "image/concepts.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include "scene_objects/shapes/sphere.hpp"
#include "vector.hpp"
#include "viewport_utils.hpp"

namespace mrl {

namespace __details {
// Postcondition:
//   - Returns the normal in opposite direction of ray_dir
constexpr direction_t normal_dir(direction_t normal, direction_t ray_dir) {
  return dot(normal.val(), ray_dir.val()) <= 0 ? normal
                                               : dir_from_unit(-normal.val());
}
} // namespace __details

template <SceneObject Object>
constexpr color_t ray_color(ray_t const &ray, Object const &world) {
  auto hit_record = hit(world, ray);
  if (hit_record) {
    // auto N = __details::normal_dir(hit_record->hit_point,
    // ray.direction).val();
    auto N = hit_record->normal.val();
    return 0.5 * (color_t{N.x, N.y, N.z} + color_t{1, 1, 1});
  }
  auto dir = ray.direction.val();
  auto a = (dir.y + 1.0) / 2;
  return (1.0 - a) * color_t{1.0, 1.0, 1.0} + a * color_t{0.0, 0.0, 0.0};
}

template <SceneObject Object, RandomAccessImage Image>
constexpr void trace(Object const &world, Image &img, camera_t const &camera,
                     camera_orientation_t const &orientation) {
  auto [u_dir, v_dir] = viewport_direction(orientation);
  auto viewport_u = u_dir * camera.viewport.width;
  auto viewport_v = v_dir * camera.viewport.height;

  auto pixel_delta_u = viewport_u / img.width();
  auto pixel_delta_v = viewport_v / img.height();

  auto viewport_top_left = viewport_topleft(viewport_u, viewport_v, orientation,
                                            camera.focal_length);

  auto pixel00_loc = viewport_top_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  for (int j = 0; j < img.height(); ++j) {
    for (int i = 0; i < img.width(); ++i) {
      auto pixel_center = pixel00_loc + i * pixel_delta_u + j * pixel_delta_v;
      auto camera_center = orientation.position;
      ray_t r{
          .origin = camera_center,
          .direction = pixel_center - orientation.position,
      };
      img.at(j, i) = ray_color(r, world);
    }
  }
}
} // namespace mrl
