#pragma once

#include "hit_record.hpp"
#include "ray.hpp"
#include <optional>
namespace mrl {
template <typename T>
concept DebugHook = requires(T &hook, ray_t const &ray,
                             std::optional<hit_record_t> const &hit) {
  hook.invoke_hit(ray, hit);
};
}
