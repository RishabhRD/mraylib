#pragma once

#include "generator/concepts.hpp"
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

template <DoubleGenerator Generator, SceneObject<Generator> Object>
std::optional<hit_record_t> hit(object_ref_t<Object> const &obj,
                                ray_t const &ray,
                                generator_view<Generator> rand) {
  return hit(*(obj.object), ray, rand);
}
} // namespace mrl
