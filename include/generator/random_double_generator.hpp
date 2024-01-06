#pragma once

#include <random>

namespace mrl {
template <typename Generator> class basic_random_double_generator {
private:
  Generator generator;

public:
  basic_random_double_generator(Generator generator_)
      : generator(std::move(generator_)) {}
  basic_random_double_generator(unsigned long seed) : generator(seed) {}
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
template <typename Generator>
basic_random_double_generator(Generator)
    -> basic_random_double_generator<Generator>;
using random_double_generator = basic_random_double_generator<std::mt19937>;
} // namespace mrl
