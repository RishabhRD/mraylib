#pragma once

#include "ray.hpp"
#include "scene_objects/concepts.hpp"

namespace mrl {
template <typename Object> struct object_ref_t {
  using object_type = Object;
  using hit_object_type = hit_object_t<Object>;
  Object *object;

  constexpr object_ref_t(Object &obj_ref) : object(&obj_ref) {}
};

template <typename Object>
object_ref_t(Object &obj_ref) -> object_ref_t<Object>;

template <SceneObject Object>
constexpr auto hit(object_ref_t<Object> const &obj, ray_t const &r) {
  return hit(*(obj.object), r);
}

template <BoundedObject Object>
constexpr bound_t get_bounds(object_ref_t<Object> const &obj) {
  return get_bounds(*(obj.object));
}
} // namespace mrl
