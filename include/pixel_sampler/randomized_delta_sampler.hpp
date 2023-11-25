#pragma once

#include "pixel_sampler/delta_sampler.hpp"
#include "std/random_double_generator.hpp"
#include "std/ranges.hpp"

namespace mrl {
struct randomized_delta_sampler
    : public delta_sampler<random_double_generator> {

  // Precondition:
  //   - sample_size_ >= 1
  randomized_delta_sampler(int sample_size_)
      : delta_sampler<random_double_generator>(
            random_double_generator{0.0, 1.0}, sample_size_) {}
};

} // namespace mrl
