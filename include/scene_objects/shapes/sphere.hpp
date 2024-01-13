#pragma once

#include "bound.hpp"
#include "direction.hpp"
#include "hit_info.hpp"
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

namespace __sphere_details {

// Postcondition:
//   - Returns the least possible t if any
constexpr std::optional<double> hit_t(sphere const &obj, ray_t const &r,
                                      interval_t const &t_range) {
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

constexpr direction_t calc_normal(sphere const &obj, point3 const &p) {
  return p - obj.center;
}

constexpr scale_2d_t calc_texture_coord(vec3 const &normal) {
  constexpr static double pi = 3.1415926535897932385;
  auto theta = acos(-normal.y);
  auto phi = atan2(-normal.z, normal.x) + pi;

  return {phi / (2 * pi), theta / pi};
}
} // namespace __sphere_details

// Postcondition:
//   - If ray doesn't intersect then return nullopt
//   - t should be the minimum possible value for which ray intersects object
//   - normal is perpendicular to hit point
//   - normal should always point outside of object
constexpr std::optional<hit_info_t> hit(sphere const &obj, ray_t const &r,
                                        interval_t const &interval) {
  auto t_opt = __sphere_details::hit_t(obj, r, interval);
  if (!t_opt)
    return std::nullopt;
  auto t = *t_opt;
  auto point = r.at(t);
  auto normal = __sphere_details::calc_normal(obj, point);
  auto scaling = __sphere_details::calc_texture_coord(normal.val());
  return hit_info_t{
      .t = t,
      .hit_point = point,
      .outward_normal = normal,
      .scale_2d = scaling,
  };
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
