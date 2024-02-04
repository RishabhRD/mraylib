#pragma once

#include "bound.hpp"
#include "direction.hpp"
#include "interval.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "scale_2d.hpp"
#include "vector.hpp"
#include <cmath>
#include <optional>

namespace mrl {
struct sphere {
  double radius;
  point3 center;
};

// Precondition:
//   - p is at surface of s
//
// Postconditon:
//   - normal points to outside the object
constexpr direction_t normal_at(sphere const &s, point3 const &p) {
  return p - s.center;
}

// Precondition:
//   - p is at surface of s
constexpr scale_2d_t scaling_2d_at(sphere const &s, point3 const &p) {
  auto const normal = normal_at(s, p).val();
  constexpr static double pi = M_PI;
  auto theta = acos(-normal.y);
  auto phi = atan2(-normal.z, normal.x) + pi;
  return {phi / (2 * pi), theta / pi};
}

// Postcondition:
//   - Returns the least possible t if any
constexpr std::optional<double>
ray_hit_distance(sphere const &obj, ray_t const &r, interval_t const &t_range) {
  auto oc = r.origin - obj.center;
  auto a = r.direction.val().length_square();
  auto half_b = dot(oc, r.direction.val());
  auto c = oc.length_square() - obj.radius * obj.radius;
  auto discriminant = half_b * half_b - a * c;
  auto discriminant_sqrt = std::sqrt(discriminant);
  if (discriminant >= 0) {
    auto t1 = (-half_b - discriminant_sqrt) / a;
    if (t_range.surrounds(t1))
      return t1;
    auto t2 = (-half_b + discriminant_sqrt) / a;
    if (t_range.surrounds(t2))
      return t2;
  }
  return std::nullopt;
}

constexpr bound_t get_bounds(sphere const &sphere) {
  return {
      .x_range = interval_t{sphere.center.x - sphere.radius,
                            sphere.center.x + sphere.radius},
      .y_range = interval_t{sphere.center.y - sphere.radius,
                            sphere.center.y + sphere.radius},
      .z_range = interval_t{sphere.center.z - sphere.radius,
                            sphere.center.z + sphere.radius},
  };
}

} // namespace mrl
