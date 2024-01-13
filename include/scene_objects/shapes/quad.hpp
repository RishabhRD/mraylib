#pragma once

#include "bound.hpp"
#include "direction.hpp"
#include "hit_info.hpp"
#include "point.hpp"
#include "scale_2d.hpp"
#include "vector.hpp"

namespace mrl {
struct quad {
  point3 corner;
  vec3 corner_side_u;
  vec3 corner_side_v;
};

// Postcondition:
//   - normal points to outisde quad
//   - need not to be unit vector
constexpr vec3 calc_normal(quad const &quad) {
  return cross(quad.corner_side_u, quad.corner_side_v);
}

constexpr std::optional<hit_info_t> hit(quad const &quad, ray_t const &r,
                                        interval_t const &interval) {
  auto n = calc_normal(quad);
  auto w = n / dot(n, n);
  auto normal = direction_t{n};
  auto denom = dot(normal.val(), r.direction.val());
  if (std::fabs(denom) < 1e-8) {
    return std::nullopt;
  }
  auto D = dot(normal.val(), quad.corner);
  auto t = (D - dot(normal.val(), r.origin)) / denom;
  if (!interval.contains(t)) {
    return std::nullopt;
  }
  auto intersection = r.at(t);
  auto planar_hitpt_vector = intersection - quad.corner;
  auto alpha = dot(w, cross(planar_hitpt_vector, quad.corner_side_v));
  auto beta = dot(w, cross(quad.corner_side_u, planar_hitpt_vector));
  if ((alpha < 0) || (1 < alpha) || (beta < 0) || (1 < beta)) {
    return std::nullopt;
  }

  return hit_info_t{
      .t = t,
      .hit_point = intersection,
      .outward_normal = normal,
      .scale_2d = scale_2d_t{alpha, beta},
  };
}

constexpr bound_t get_bounds(quad const &quad) {
  return pad_bounds(bound_from_diagonal_points(
      quad.corner, quad.corner + quad.corner_side_u + quad.corner_side_v));
}

} // namespace mrl
