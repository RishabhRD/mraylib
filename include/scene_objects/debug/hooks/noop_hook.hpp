#pragma once

#include "hit_context.hpp"
#include "ray.hpp"
#include <optional>
namespace mrl {
struct noop_hook {
  constexpr void invoke_hit(ray_t const &,
                            std::optional<hit_context_t> const &) const {}
};
} // namespace mrl
