#pragma once

#include "direction.hpp"
#include "equation.hpp"
#include "hit_record.hpp"
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

// Postcondition:
//   - Returns the least possible t if any
constexpr std::optional<double> hit_t(sphere_obj_t const &obj, ray_t const &r) {
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

constexpr direction_t calc_normal(sphere_obj_t const &obj, point3 const &p) {
  return p - obj.center;
}

// Postcondition:
//   - If ray doesn't intersect then return nullopt
//   - t should be the minimum possible value for which ray intersects object
constexpr std::optional<hit_record_t> hit(sphere_obj_t const &obj,
                                          ray_t const &r) {
  auto t_opt = hit_t(obj, r);
  return t_opt.transform([&obj, &r](double t) {
    auto point = r.at(t);
    return hit_record_t{
        .t = t,
        .hit_point = point,
        .normal = calc_normal(obj, point),
    };
  });
}

} // namespace mrl
