#pragma once

#include "direction.hpp"
#include "equation.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <cmath>
#include <optional>
#include <vector>

namespace mrl {
struct sphere_t {
  double radius;
};

struct sphere_obj_t {
  sphere_t sphere;
  point3 center;
};

constexpr std::optional<double> hit(sphere_obj_t const &sphere,
                                    ray_t const &r) {
  auto oc = r.origin - sphere.center;
  auto a = dot(r.direction.val(), r.direction.val());
  auto b = 2.0 * dot(oc, r.direction.val());
  auto c = dot(oc, oc) - sphere.sphere.radius * sphere.sphere.radius;
  auto discriminant = calc_discriminant(a, b, c);
  auto discriminant_sqrt = std::sqrt(discriminant);
  if (discriminant == 0) {
    auto t1 = -b / (2 * a);
    if (t1 >= 0)
      return t1;
  } else if (discriminant > 0) {
    auto t1 = (-b - discriminant_sqrt) / (2 * a);
    if (t1 >= 0)
      return t1;
    auto t2 = (-b + discriminant_sqrt) / (2 * a);
    if (t2 >= 0)
      return t2;
  }
  return std::nullopt;
}

constexpr direction_t normal(sphere_obj_t const &sphere, point3 const &p) {
  return p - sphere.center;
}
} // namespace mrl
