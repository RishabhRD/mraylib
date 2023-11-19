#pragma once

#include "hit_record.hpp"
#include "ray.hpp"
#include <optional>
namespace mrl {
struct noop_hook {
  constexpr void invoke_hit(ray_t const &,
                            std::optional<hit_record_t> const &) const {}
};
} // namespace mrl
