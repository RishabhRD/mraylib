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

constexpr std::optional<double> hit(sphere_obj_t const &obj, ray_t const &r) {
  auto oc = r.origin - obj.center;
  auto a = r.direction.val().length_square();
  auto half_b = dot(oc, r.direction.val());
  auto c = oc.length_square() - obj.sphere.radius * obj.sphere.radius;
  auto discriminant = half_b * half_b - a * c;
  auto discriminant_sqrt = std::sqrt(discriminant);
  if (discriminant >= 0) {
    auto t1 = (-half_b - discriminant_sqrt) / a;
    if (t1 >= 0)
      return t1;
    auto t2 = (-half_b + discriminant_sqrt) / a;
    if (t2 >= 0)
      return t2;
  }
  return std::nullopt;
}

constexpr direction_t normal(sphere_obj_t const &obj, point3 const &p) {
  return p - obj.center;
}
} // namespace mrl
