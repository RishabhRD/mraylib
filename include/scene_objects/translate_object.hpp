#pragma once

#include "bound.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "hit_context.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include "vector.hpp"
namespace mrl {
template <typename Object> struct translate_object {
  using object_type = Object;
  Object internal_object;
  vec3 offset;

  translate_object(object_type internal_object_, vec3 offset_)
      : internal_object(std::move(internal_object_)), offset(offset_) {}
};

template <typename Object>
translate_object(Object, vec3) -> translate_object<Object>;

template <DoubleGenerator Generator, Hittable<Generator> Object>
constexpr std::optional<hit_context_t> hit(translate_object<Object> const &obj,
                                           ray_t r, interval_t const &interval,
                                           generator_view<Generator> rand) {
  r.origin -= obj.offset;
  auto res = hit(obj.internal_object, r, interval, rand);
  if (!res)
    return std::nullopt;
  res->hit_info.hit_point += obj.offset;
  if (res->scatter_info)
    res->scatter_info->scattered_ray.origin += obj.offset;
  return res;
}

template <BoundedObject Object>
constexpr bound_t get_bounds(translate_object<Object> const &obj) {
  return shift(get_bounds(obj.internal_object), obj.offset);
}
} // namespace mrl
