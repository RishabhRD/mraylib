#pragma once

#include "bound.hpp"
#include "direction.hpp"
#include "generator/concepts.hpp"
#include "hit_record.hpp"
#include "materials/concept.hpp"
#include "point.hpp"
#include "textures/texture_coord.hpp"
#include "vector.hpp"

namespace mrl {
struct quad_t {
  point3 corner;
  vec3 corner_side_u;
  vec3 corner_side_v;
};

constexpr bound_t bounding_box(quad_t const &quad) {
  return pad_bounds(bound_from_diagonal_points(
      quad.corner, quad.corner + quad.corner_side_u + quad.corner_side_v));
}

// Postcondition:
//   - normal points to outisde quad
//   - need not to be unit vector
constexpr vec3 calc_normal(quad_t const &quad) {
  return cross(quad.corner_side_u, quad.corner_side_v);
}

template <typename Material> struct quad_obj_t {
  quad_t shape;
  Material material;

  quad_obj_t(quad_t quadrilateral_, Material material_)
      : shape(quadrilateral_), material(std::move(material_)) {}

  quad_obj_t(point3 corner_, vec3 corner_side_u_, vec3 corner_side_v_,
             Material material_)
      : shape(quad_t{corner_, corner_side_u_, corner_side_v_}),
        material(std::move(material_)) {}

  // TODO: Think of caching normal and bounds with immutability
};

template <typename Material>
quad_obj_t(quad_t, Material) -> quad_obj_t<Material>;

template <typename Material>
quad_obj_t(point3, vec3, vec3, Material) -> quad_obj_t<Material>;

template <DoubleGenerator Generator, Material<Generator> material_t>
constexpr std::optional<hit_record_t>
hit(quad_obj_t<material_t> const &obj, ray_t const &r,
    interval_t const &interval, generator_view<Generator> rand) {
  auto n = calc_normal(obj.shape);
  auto w = n / dot(n, n);
  auto normal = direction_t{n};
  auto denom = dot(normal.val(), r.direction.val());
  if (std::fabs(denom) < 1e-8) {
    return std::nullopt;
  }
  auto D = dot(normal.val(), obj.shape.corner);
  auto t = (D - dot(normal.val(), r.origin)) / denom;
  if (!interval.contains(t)) {
    return std::nullopt;
  }
  auto intersection = r.at(t);
  auto planar_hitpt_vector = intersection - obj.shape.corner;
  auto alpha = dot(w, cross(planar_hitpt_vector, obj.shape.corner_side_v));
  auto beta = dot(w, cross(obj.shape.corner_side_u, planar_hitpt_vector));
  if ((alpha < 0) || (1 < alpha) || (beta < 0) || (1 < beta)) {
    return std::nullopt;
  }

  return hit_record_t{
      .t = t,
      .hit_point = intersection,
      .normal = normal,
      .scattering = scatter(obj.material, r, intersection,
                            texture_coord_t{alpha, beta}, normal, rand),
  };
}

template <typename Material>
constexpr bound_t get_bounds(quad_obj_t<Material> const &obj) {
  return bounding_box(obj.shape);
}

} // namespace mrl
