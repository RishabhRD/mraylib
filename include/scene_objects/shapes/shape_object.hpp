#pragma once

#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "hit_context.hpp"
#include "hit_info.hpp"
#include "interval.hpp"
#include "materials/concept.hpp"
#include "materials/emit_info.hpp"
#include "ray.hpp"
#include "scene_objects/shapes/concepts.hpp"

namespace mrl {
template <DoubleGenerator Generator, typename material_t>
std::optional<scatter_info_t>
scatter_light(material_t const &material, ray_t const &ray,
              hit_info_t const &hit_info, generator_view<Generator> rand) {
  if constexpr (LightScatterer<material_t, Generator>) {
    return scatter(material, ray, hit_info, rand);
  } else {
    return std::nullopt;
  }
}

template <DoubleGenerator Generator, typename material_t>
std::optional<emit_info_t> emit_light(material_t const &material,
                                      hit_info_t const &hit_info,
                                      generator_view<Generator> rand) {
  if constexpr (LightEmitter<material_t, Generator>) {
    return emit(material, hit_info, rand);
  } else {
    return std::nullopt;
  }
}

template <Shape shape_t, typename material_t> struct shape_object {
  using shape_type = shape_t;
  using material_type = material_t;

  shape_type shape;
  material_type material;

  constexpr shape_object(shape_type shape_, material_type material_)
      : shape(std::move(shape_)), material(std::move(material_)) {}
};

template <Shape shape_t, typename material_t>
shape_object(shape_t, material_t) -> shape_object<shape_t, material_t>;

template <DoubleGenerator Generator, Shape shape_t, typename material_t>
constexpr std::optional<hit_context_t>
hit(shape_object<shape_t, material_t> const &obj, ray_t const &ray,
    interval_t const &interval, generator_view<Generator> rand) {
  auto hit_rec = hit(obj.shape, ray, interval);
  if (!hit_rec) {
    return std::nullopt;
  }
  return hit_context_t{
      .hit_info = *hit_rec,
      .scatter_info = scatter_light(obj.material, ray, *hit_rec, rand),
      .emit_info = emit_light(obj.material, *hit_rec, rand),
  };
}

template <Shape shape, typename material_t>
constexpr bound_t get_bounds(shape_object<shape, material_t> const &obj) {
  return get_bounds(obj.shape);
}
} // namespace mrl
