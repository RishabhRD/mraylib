#pragma once

#include "pixel_sampler/sampler_args.hpp"
#include <ranges>

namespace mrl {
struct identity_sampler {
  constexpr auto sample(sampler_args_t const &args) const {
    return std::views::single(args.point);
  }
};
} // namespace mrl
