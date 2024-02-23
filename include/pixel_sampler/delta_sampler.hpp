#pragma once

#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "pixel_sampler/sampler_args.hpp"
#include "point.hpp"
#include "std/functional.hpp"
#include "std/ranges.hpp"
#include "vector.hpp"

namespace mrl {
constexpr point3 make_sampling_pixel_point(vec3 const &pixel_delta_u,
                                           vec3 const &pixel_delta_v,
                                           point3 const &pixel_center,
                                           std::pair<double, double> delta) {
  delta.first -= 0.5;
  delta.second -= 0.5;
  return pixel_center + (delta.first * pixel_delta_u) +
         (delta.second * pixel_delta_v);
}

// Precondition:
//   - sample_size >= 1
template <DoubleGenerator Generator>
constexpr auto make_sampling_pixel_points(int sample_size,
                                          sampler_args_t const &args,
                                          generator_view<Generator> gen_delta) {
  auto deltas = views::repeat_fn(std::bind_front(gen_delta, 0.0, 1.0));
  namespace vw = std::views;
  auto pnt_sampling_fn = std::bind_front(
      lift(make_sampling_pixel_point), args.pixel_delta_u, args.pixel_delta_v);
  return vw::zip_transform(pnt_sampling_fn, vw::repeat(args.point),
                           views::chunk_pair(deltas)) //
         | vw::take(sample_size);
}

struct delta_sampler {
  int sample_size;

  // Precondition:
  //   - sample_size_ >= 1
  constexpr delta_sampler(int sample_size_) : sample_size(sample_size_) {}

  template <DoubleGenerator Generator>
  constexpr auto operator()(sampler_args_t const &args,
                            generator_view<Generator> gen_delta) const {
    return make_sampling_pixel_points(sample_size, args, gen_delta);
  }
};
} // namespace mrl
