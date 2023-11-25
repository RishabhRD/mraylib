#pragma once

#include "functional.hpp"
#include "pixel_sampler/concepts.hpp"
#include "pixel_sampler/sampler_args.hpp"
#include "point.hpp"
#include "std/random_double_generator.hpp"
#include "std/ranges.hpp"
#include "vector.hpp"
#include <concepts>
#include <type_traits>

namespace mrl {
constexpr point3 make_sampling_pixel_point(sampler_args_t const &args,
                                           std::pair<double, double> delta) {
  delta.first -= 0.5;
  delta.second -= 0.5;
  return args.point + (delta.first * args.pixel_delta_u) +
         (delta.second * args.pixel_delta_v);
}
// Precondition:
//   - sample_size >= 1
// template <typename DeltaRange>
// requires RangeValueType<DeltaRange, double>
// constexpr auto make_sampling_pixel_points(int sample_size, DeltaRange
// &deltas,
//                                           sampler_args_t const &args) {
//   namespace vw = std::views;
//   auto pnt_sampling_fn = std::bind_front(
//       lift(make_sampling_pixel_point), args.pixel_delta_u,
//       args.pixel_delta_v);
//   return vw::repeat(args.point)                                          //
//          | vw::zip_transform(views::chunk_pair(deltas), pnt_sampling_fn) //
//          | vw::take(sample_size);
// }

// Precondition:
//   - sample_size >= 1
template <typename DeltaRange>
  requires RangeValueType<DeltaRange, double>
constexpr auto make_sampling_pixel_points(int sample_size, DeltaRange &deltas,
                                          sampler_args_t const &args) {
  std::vector<point3> res;
  auto delta_cur = std::ranges::begin(deltas);
  for (int i = 0; i < sample_size; ++i) {
    auto first = *delta_cur;
    ++delta_cur;
    auto second = *delta_cur;
    ++delta_cur;
    res.push_back(make_sampling_pixel_point(args, {first, second}));
  }
  return res;
}

template <std::invocable DeltaGenerator>
  requires std::same_as<std::invoke_result_t<DeltaGenerator>, double>
struct delta_sampler {
  using delta_range_t =
      std::invoke_result_t<decltype(views::repeat_fn), DeltaGenerator>;
  delta_range_t deltas;
  int sample_size;

  // Precondition:
  //   - std::invoke(generator) should be in [0, 1)
  //   - sample_size_ >= 1
  constexpr delta_sampler(DeltaGenerator generator, int sample_size_)
      : deltas(std::invoke(views::repeat_fn, std::move(generator))),
        sample_size(sample_size_) {}

  constexpr auto sample(sampler_args_t const &args) {
    return make_sampling_pixel_points(sample_size, deltas, args);
  }
};
} // namespace mrl
