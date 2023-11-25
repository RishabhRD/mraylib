#pragma once

#include <random>

namespace mrl {
class random_double_generator {
private:
  std::uniform_real_distribution<double> distribution;
  std::mt19937 generator;

public:
  random_double_generator(double min, double max) : distribution(min, max) {}

  double operator()() { return distribution(generator); }
};
} // namespace mrl
