#pragma once

#include "bound.hpp"
#include "direction.hpp"
#include "interval.hpp"
#include "point.hpp"
#include "ray.hpp"
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

// Postcondition:
//   - normal points to outisde quad
constexpr direction_t normal_at(quad const &q, point3 const &) {
  return calc_normal(q);
}

// Precondition:
//   - p should be at surface of q
constexpr scale_2d_t scaling_2d_at(quad const &q, point3 const &p) {
  auto const n = calc_normal(q);
  auto const w = n / dot(n, n);
  auto const planar_hitpt_vector = p - q.corner;
  auto const alpha = dot(w, cross(planar_hitpt_vector, q.corner_side_v));
  auto const beta = dot(w, cross(q.corner_side_u, planar_hitpt_vector));
  return {alpha, beta};
}

constexpr std::optional<double> ray_hit_distance(quad const &q, ray_t const &r,
                                                 interval_t const &interval) {
  auto n = calc_normal(q);
  auto normal = direction_t{n};
  auto denom = dot(normal.val(), r.direction.val());
  if (std::fabs(denom) < 1e-8) {
    return std::nullopt;
  }
  auto D = dot(normal.val(), q.corner);
  auto t = (D - dot(normal.val(), r.origin)) / denom;
  if (!interval.contains(t)) {
    return std::nullopt;
  }
  auto scaling = scaling_2d_at(q, r.at(t));
  auto alpha = scaling.x_scale();
  auto beta = scaling.y_scale();
  if ((alpha < 0) || (1 < alpha) || (beta < 0) || (1 < beta)) {
    return std::nullopt;
  }
  return t;
}

constexpr bound_t get_bounds(quad const &quad) {
  return pad_bounds(bound_from_diagonal_points(
      quad.corner, quad.corner + quad.corner_side_u + quad.corner_side_v));
}

} // namespace mrl
