#pragma once

#include "equation.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <cmath>
#include <vector>

namespace mrl {
struct sphere_t {
  double radius;
};

struct sphere_obj_t {
  sphere_t sphere;
  point3 center;
};

inline std::vector<double> hit(sphere_obj_t const &sphere, ray_t const &r) {
  std::vector<double> res;
  auto oc = r.origin - sphere.center;
  auto a = dot(r.direction.val(), r.direction.val());
  auto b = 2.0 * dot(oc, r.direction.val());
  auto c = dot(oc, oc) - sphere.sphere.radius * sphere.sphere.radius;
  auto discriminant = calc_discriminant(a, b, c);
  auto discriminant_sqrt = std::sqrt(discriminant);
  if (discriminant == 0) {
    res.push_back(-b / (2 * a));
  } else if (discriminant > 0) {
    res.push_back((-b - discriminant_sqrt) / (2 * a));
    res.push_back((-b + discriminant_sqrt) / (2 * a));
  }
  return res;
}
} // namespace mrl
