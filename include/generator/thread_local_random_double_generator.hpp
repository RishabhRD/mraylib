#pragma once

#include <random>

namespace mrl {
struct thread_local_random_double_generator {
  unsigned long random_seed;
  thread_local_random_double_generator(unsigned long random_seed_)
      : random_seed(random_seed_) {}
  // Precondition:
  //   - min < max
  //
  // Postcondition:
  //   - generates a random value in range [min, max)
  double operator()(double min, double max) {
    thread_local static std::mt19937 generator{random_seed};
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
  }
};
} // namespace mrl
