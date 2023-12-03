#pragma once

#include <random>

namespace mrl {
struct thread_local_random_double_generator {
  // Precondition:
  //   - min < max
  //
  // Postcondition:
  //   - generates a random value in range [min, max)
  double operator()(double min, double max) {
    thread_local static std::mt19937 generator;
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
  }
};
} // namespace mrl
