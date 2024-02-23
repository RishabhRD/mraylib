#pragma once

#include "bound.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "hit_info.hpp"
#include "interval.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include "scene_objects/traits.hpp"
#include "vector.hpp"

namespace mrl {
template <typename Object> struct translate_hit_object {
  hit_object_t<Object> hit_obj;
  vec3 offset;
};

template <typename Object> struct translate_object {
  using object_type = Object;
  using hit_object_type = translate_hit_object<Object>;
  Object internal_object;
  vec3 offset;

  constexpr translate_object(object_type internal_object_, vec3 offset_)
      : internal_object(std::move(internal_object_)), offset(offset_) {}
};

template <typename Object>
translate_object(Object, vec3) -> translate_object<Object>;

template <typename Object>
constexpr auto normal_at(translate_hit_object<Object> const &o,
                         point3 const &p) {
  return normal_at(o.hit_obj, p - o.offset);
}

template <typename Object>
constexpr auto scaling_2d_at(translate_hit_object<Object> const &o,
                             point3 const &p) {
  return scaling_2d_at(o.hit_obj, p - o.offset);
}

template <DoubleGenerator Generator, typename Object>
constexpr auto scattering_for(translate_hit_object<Object> const &o, ray_t r,
                              double hit_distance,
                              generator_view<Generator> rand) {
  r.origin -= o.offset;
  return scattering_for(o.hit_obj, r, hit_distance, rand);
}

template <DoubleGenerator Generator, typename Object>
constexpr auto emission_at(translate_hit_object<Object> const &o,
                           point3 const &p, generator_view<Generator> rand) {
  return emission_at(o.hit_obj, p - o.offset, rand);
}

template <SceneObject Object>
constexpr std::optional<hit_info_t<translate_hit_object<Object>>>
hit(translate_object<Object> const &obj, ray_t r, interval_t const &interval) {
  r.origin -= obj.offset;
  auto internal_hit = hit(obj.internal_object, r, interval);
  if (!internal_hit)
    return std::nullopt;
  return hit_info_t<hit_object_t<translate_object<Object>>>{
      internal_hit->hit_distance,
      {std::move(internal_hit->hit_object), obj.offset}};
}

template <BoundedObject Object>
constexpr bound_t get_bounds(translate_object<Object> const &obj) {
  return shift(get_bounds(obj.internal_object), obj.offset);
}
} // namespace mrl
