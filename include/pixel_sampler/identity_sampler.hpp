#pragma once

#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "pixel_sampler/sampler_args.hpp"
#include <ranges>

namespace mrl {
struct identity_sampler {
  template <DoubleGenerator Generator>
  constexpr auto operator()(sampler_args_t const &args,
                            generator_view<Generator>) const {
    return std::views::single(args.point);
  }
};
} // namespace mrl
