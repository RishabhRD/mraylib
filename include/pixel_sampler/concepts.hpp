#pragma once

#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "pixel_sampler/sampler_args.hpp"
#include "point.hpp"
#include "std/ranges.hpp"

namespace mrl {
namespace __details {
template <typename T>
concept PixelSamplingResult =
    std::ranges::input_range<T> && RangeValueType<T, point3>;
}

template <typename Sampler, typename random_generator>
concept PixelSampler =
    DoubleGenerator<random_generator> &&
    requires(Sampler const &sampler, sampler_args_t const &args,
             generator_view<random_generator> gen_random) {
      // Postcondition:
      //   - Let's say returned range is rng
      //   - size(rng) >= 1
      //   - All points in rng should lie on viewport plane
      { sampler(args, gen_random) } -> __details::PixelSamplingResult;
    };
} // namespace mrl
