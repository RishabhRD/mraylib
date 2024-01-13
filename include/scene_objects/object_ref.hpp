#pragma once

#include "generator/concepts.hpp"
#include "hit_context.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include <optional>

namespace mrl {
template <typename Object> struct object_ref_t {
  Object *object;

  constexpr object_ref_t(Object &obj_ref) : object(&obj_ref) {}
};

template <typename Object>
object_ref_t(Object &obj_ref) -> object_ref_t<Object>;

template <DoubleGenerator Generator, SceneObject<Generator> Object>
constexpr std::optional<hit_context_t> hit(object_ref_t<Object> const &obj,
                                           ray_t const &ray,
                                           generator_view<Generator> rand) {
  return hit(*(obj.object), ray, rand);
}

template <BoundedObject Object>
constexpr bound_t get_bounds(object_ref_t<Object> const &obj) {
  return get_bounds(*(obj.object));
}
} // namespace mrl
