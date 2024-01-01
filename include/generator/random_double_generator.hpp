#pragma once

#include <random>

namespace mrl {
class random_double_generator {
private:
  std::mt19937 generator;

public:
  random_double_generator(unsigned long seed) : generator(seed) {}
  // Precondition:
  //   - min < max
  //
  // Postcondition:
  //   - generates a random value in range [min, max)
  double operator()(double min, double max) {
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
  }
};
} // namespace mrl
