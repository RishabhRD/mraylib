#pragma once

#include "angle.hpp"
#include "bound.hpp"
#include "point.hpp"
#include "ray.hpp"
#include <algorithm>
#include <functional>

namespace mrl {
constexpr ray_t rotate(ray_t const &r, ray_t const &axis,
                       angle_t const &angle) {
  vec3 translated_origin = r.origin - axis.origin;

  auto rdir = r.direction.val();
  auto adir = axis.direction.val();

  double angle_rad = radians(angle);

  double cos_theta = std::cos(angle_rad);
  double sin_theta = std::sin(angle_rad);
  vec3 rotated_direction = rdir * cos_theta + cross(adir, rdir) * sin_theta +
                           adir * dot(adir, rdir) * (1 - cos_theta);

  vec3 rotated_origin = axis.origin + (cos_theta * translated_origin) +
                        cross(adir, translated_origin) * sin_theta +
                        adir * dot(adir, translated_origin) * (1 - cos_theta);

  return {rotated_origin, rotated_direction};
}

constexpr point3 rotate(point3 const &p, ray_t const &axis,
                        angle_t const &angle) {
  double angle_rad = radians(angle);
  double cos_theta = std::cos(angle_rad);
  double sin_theta = std::sin(angle_rad);

  double ux = axis.direction.val().x;
  double uy = axis.direction.val().y;
  double uz = axis.direction.val().z;
  double one_minus_cos_theta = 1 - cos_theta;

  double rotated_x = p.x * (cos_theta + ux * ux * one_minus_cos_theta) +
                     p.y * (ux * uy * one_minus_cos_theta - uz * sin_theta) +
                     p.z * (ux * uz * one_minus_cos_theta + uy * sin_theta);

  double rotated_y = p.x * (uy * ux * one_minus_cos_theta + uz * sin_theta) +
                     p.y * (cos_theta + uy * uy * one_minus_cos_theta) +
                     p.z * (uy * uz * one_minus_cos_theta - ux * sin_theta);

  double rotated_z = p.x * (uz * ux * one_minus_cos_theta - uy * sin_theta) +
                     p.y * (uz * uy * one_minus_cos_theta + ux * sin_theta) +
                     p.z * (cos_theta + uz * uz * one_minus_cos_theta);

  return {rotated_x, rotated_y, rotated_z};
}

constexpr bound_t rotate(bound_t const &bound, ray_t const &axis,
                         angle_t const &angle) {
  namespace rng = std::ranges;
  auto x_range = bound.x_range;
  auto y_range = bound.y_range;
  auto z_range = bound.z_range;

  vec3 corners[8] = {
      {x_range.min, y_range.min, z_range.min},
      {x_range.min, y_range.min, z_range.max},
      {x_range.min, y_range.max, z_range.min},
      {x_range.min, y_range.max, z_range.max},
      {x_range.max, y_range.min, z_range.min},
      {x_range.max, y_range.min, z_range.max},
      {x_range.max, y_range.max, z_range.min},
      {x_range.max, y_range.max, z_range.max},
  };

  auto rotate_point = [&axis, &angle](point3 const &p) {
    return rotate(p, axis, angle);
  };

  rng::transform(corners, std::begin(corners), rotate_point);

  auto [min_x, max_x] =
      rng::minmax_element(corners, std::less<>{}, std::mem_fn(&vec3::x));
  auto [min_y, max_y] =
      rng::minmax_element(corners, std::less<>{}, std::mem_fn(&vec3::y));
  auto [min_z, max_z] =
      rng::minmax_element(corners, std::less<>{}, std::mem_fn(&vec3::z));
  return bound_from_diagonal_points({min_x->x, min_y->y, min_z->z},
                                    {max_x->x, max_y->y, max_z->z});
}
} // namespace mrl
