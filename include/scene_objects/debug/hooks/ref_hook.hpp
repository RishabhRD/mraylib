#pragma once

#include "hit_context.hpp"
#include "ray.hpp"
#include <optional>

namespace mrl {
template <typename Hook> class ref_hook {
private:
  Hook *hook;

public:
  ref_hook(Hook &h) : hook(&h) {}

  constexpr void invoke_hit(ray_t const &ray,
                            std::optional<hit_context_t> const &rec) const {
    hook->invoke_hit(ray, rec);
  }
};

template <typename Hook> ref_hook(Hook &hook) -> ref_hook<Hook>;
} // namespace mrl
