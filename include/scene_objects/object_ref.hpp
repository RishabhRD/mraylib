#pragma once

#include "hit_record.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include <optional>
namespace mrl {
template <typename Object> struct object_ref_t {
  Object *object;

  object_ref_t(Object &obj_ref) : object(&obj_ref) {}
};

template <typename Object>
object_ref_t(Object &obj_ref) -> object_ref_t<Object>;

template <SceneObject Object>
std::optional<hit_record_t> hit(object_ref_t<Object> const &obj,
                                ray_t const &ray) {
  return hit(*(obj.object), ray);
}
} // namespace mrl
