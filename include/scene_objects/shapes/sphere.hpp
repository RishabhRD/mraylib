#pragma once

#include "direction.hpp"
#include "hit_record.hpp"
#include "interval.hpp"
#include "materials/concept.hpp"
#include "normal.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <cmath>
#include <optional>

namespace mrl {
// TODO: I don't know... this is required or not
// struct sphere_t {
//   double radius;
// };

template <Material material_t> struct sphere_obj_t {
  double radius;
  point3 center;
  material_t material;

  constexpr sphere_obj_t(double radius_, point3 center_, material_t material_)
      : radius(radius_), center(center_), material(std::move(material_)) {}
};

template <Material material_t>
sphere_obj_t(double, point3, material_t) -> sphere_obj_t<material_t>;

// Postcondition:
//   - Returns the least possible t if any
template <Material material_t>
constexpr std::optional<double> hit_t(sphere_obj_t<material_t> const &obj,
                                      ray_t const &r,
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

template <Material material_t>
constexpr direction_t calc_normal(sphere_obj_t<material_t> const &obj,
                                  point3 const &p) {
  return p - obj.center;
}

// Postcondition:
//   - If ray doesn't intersect then return nullopt
//   - t should be the minimum possible value for which ray intersects object
//   - normal is st dot(normal, ray.dir) <= 0
template <Material material_t>
constexpr std::optional<hit_record_t> hit(sphere_obj_t<material_t> const &obj,
                                          ray_t const &r,
                                          interval_t const &interval) {
  auto t_opt = hit_t(obj, r, interval);
  return t_opt.transform([&obj, &r](double t) {
    auto point = r.at(t);
    auto normal = normal_dir(calc_normal(obj, point), r.direction);
    return hit_record_t{
        .t = t,
        .hit_point = point,
        .normal = calc_normal(obj, point),
        .scattering = scatter(obj.material, r, point, normal),
    };
  });
}

} // namespace mrl
