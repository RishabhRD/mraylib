#pragma once

#include "bound.hpp"
#include "direction.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "hit_record.hpp"
#include "interval.hpp"
#include "materials/concept.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "textures/texture_coord.hpp"
#include "vector.hpp"
#include <cmath>
#include <optional>

namespace mrl {
// TODO: I don't know... this is required or not
// struct sphere_t {
//   double radius;
// };

template <typename material_t> struct sphere_obj_t {
  double radius;
  point3 center;
  material_t material;

  constexpr sphere_obj_t(double radius_, point3 center_, material_t material_)
      : radius(radius_), center(center_), material(std::move(material_)) {}
};

template <typename material_t>
sphere_obj_t(double, point3, material_t) -> sphere_obj_t<material_t>;

namespace __sphere_details {

// Postcondition:
//   - Returns the least possible t if any
template <typename material_t>
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

template <typename material_t>
constexpr direction_t calc_normal(sphere_obj_t<material_t> const &obj,
                                  point3 const &p) {
  return p - obj.center;
}

constexpr texture_coord_t calc_texture_coord(vec3 const &normal) {
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
template <DoubleGenerator Generator, Material<Generator> material_t>
constexpr std::optional<hit_record_t>
hit(sphere_obj_t<material_t> const &obj, ray_t const &r,
    interval_t const &interval, generator_view<Generator> rand) {
  auto t_opt = __sphere_details::hit_t(obj, r, interval);
  return t_opt.transform([&obj, &r, rand](double t) {
    auto point = r.at(t);
    auto normal = __sphere_details::calc_normal(obj, point);
    return hit_record_t{
        .t = t,
        .hit_point = point,
        .normal = normal,
        .scattering = scatter(
            obj.material, r, point,
            __sphere_details::calc_texture_coord(normal.val()), normal, rand),
    };
  });
}

template <typename Material>
bound_t get_bounds(sphere_obj_t<Material> const &sphere) {
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
