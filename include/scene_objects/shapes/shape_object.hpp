#pragma once

#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "hit_info.hpp"
#include "interval.hpp"
#include "materials/concept.hpp"
#include "materials/emit_info.hpp"
#include "materials/material_context.hpp"
#include "materials/scatter_info.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "scene_objects/shapes/concepts.hpp"

namespace mrl {
template <Shape shape_t, typename material_t> struct shape_object;
template <Shape shape_t, typename material_t> struct shape_hit_object {
  shape_object<shape_t, material_t> const *obj;
};

template <Shape shape_t, typename material_t> struct shape_object {
  using shape_type = shape_t;
  using material_type = material_t;
  using hit_object_type = shape_hit_object<shape_t, material_t>;

  shape_type shape;
  material_type material;

  constexpr shape_object(shape_type shape_, material_type material_)
      : shape(std::move(shape_)), material(std::move(material_)) {}
};

template <Shape shape_t, typename material_t>
shape_object(shape_t, material_t) -> shape_object<shape_t, material_t>;

template <Shape shape_t, typename material>
constexpr auto normal_at(shape_hit_object<shape_t, material> const &o,
                         point3 const &p) {
  return normal_at(o.obj->shape, p);
}

template <Shape shape_t, typename material>
constexpr auto scaling_2d_at(shape_hit_object<shape_t, material> const &o,
                             point3 const &p) {
  return scaling_2d_at(o.obj->shape, p);
}

template <DoubleGenerator Generator, Shape shape_t, typename material_t>
constexpr std::optional<scatter_info_t>
scattering_for(shape_hit_object<shape_t, material_t> const &o, ray_t const &r,
               double hit_distance, generator_view<Generator> rand) {
  if constexpr (LightScatterer<material_t, Generator>) {
    auto const &material = o.obj->material;
    auto ctx = make_scattering_context(o, r, hit_distance);
    return scatter(material, ctx, rand);
  } else {
    return std::nullopt;
  }
}

template <DoubleGenerator Generator, Shape shape_t, typename material_t>
constexpr std::optional<emit_info_t>
emission_at(shape_hit_object<shape_t, material_t> const &o, point3 const &p,
            generator_view<Generator> rand) {
  if constexpr (LightEmitter<material_t, Generator>) {
    auto const &material = o.obj->material;
    auto ctx = make_emission_context(o, p);
    return emit(material, ctx, rand);
  } else {
    return std::nullopt;
  }
}

template <Shape shape_t, typename material_t>
constexpr std::optional<hit_info_t<shape_hit_object<shape_t, material_t>>>
hit(shape_object<shape_t, material_t> const &obj, ray_t const &ray,
    interval_t const &interval) {
  auto hit_dist_opt = ray_hit_distance(obj.shape, ray, interval);
  if (!hit_dist_opt) {
    return std::nullopt;
  }
  return hit_info_t<shape_hit_object<shape_t, material_t>>{
      *hit_dist_opt, shape_hit_object<shape_t, material_t>{&obj}};
}

template <Shape shape, typename material_t>
constexpr bound_t get_bounds(shape_object<shape, material_t> const &obj) {
  return get_bounds(obj.shape);
}
} // namespace mrl
