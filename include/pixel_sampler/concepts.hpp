#pragma once

#include "pixel_sampler/sampler_args.hpp"
#include "point.hpp"
#include "std/ranges.hpp"
#include "vector.hpp"
#include <ranges>

namespace mrl {
namespace __details {
template <typename T>
concept PixelSamplingResult =
    std::ranges::input_range<T> && RangeValueType<T, point3>;
}

template <typename T>
concept PixelSampler = requires(T sampler, sampler_args_t const &args) {
  // Postcondition:
  //   - Let's say returned range is rng
  //   - size(rng) >= 1
  //   - All points in rng should lie on viewport plane
  { sampler.sample(args) } -> __details::PixelSamplingResult;
};
} // namespace mrl
