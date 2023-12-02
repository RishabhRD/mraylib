#pragma once

#include "generator/concepts.hpp"
#include "scene_objects/concepts.hpp"
#include "scene_objects/debug/hooks/concepts.hpp"
namespace mrl {
template <typename Object, DebugHook Hook> struct debug_obj_t {
  Object object;
  mutable Hook hook;

  debug_obj_t(Object obj, Hook h)
      : object(std::move(obj)), hook(std::move(h)) {}
};

template <typename Object, DebugHook Hook>
debug_obj_t(Object obj, Hook h) -> debug_obj_t<Object, Hook>;

template <DoubleGenerator Generator, SceneObject<Generator> Object,
          typename Hook>
constexpr std::optional<hit_record_t> hit(debug_obj_t<Object, Hook> const &obj,
                                          ray_t const &r) {
  auto res = hit(obj.object, r);
  obj.hook.invoke_hit(r, res);
  return res;
}

} // namespace mrl
