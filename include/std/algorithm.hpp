#pragma once

#include <algorithm>
#include <iterator>
#include <numeric>
#include <type_traits>

namespace mrl {
// Postcondition:
//   - [begin, end) would contain a random permutation from [0, end - begin)
template <std::random_access_iterator Iter, typename Generator>
  requires std::is_default_constructible_v<std::iter_value_t<Iter>> &&
           std::incrementable<std::iter_value_t<Iter>>
constexpr void random_permutation(Iter begin, Iter end, Generator &&gen) {
  using value_type = std::iter_value_t<Iter>;
  std::iota(begin, end, value_type{});
  std::shuffle(begin, end, gen);
}

// Postcondition:
//   - Returns a tri-interpolation of the point (u, v, w) in cube
constexpr double tri_interpolate(double cube[2][2][2], double u, double v,
                                 double w) {
  double accum{0.0};
  for (int i_{0}; i_ < 2; ++i_) {
    for (int j_{0}; j_ < 2; ++j_) {
      for (int k_{0}; k_ < 2; ++k_) {
        auto i = static_cast<double>(i_);
        auto j = static_cast<double>(j_);
        auto k = static_cast<double>(k_);
        accum += (i * u + (1 - i) * (1 - u)) * (j * v + (1 - j) * (1 - v)) *
                 (k * w + (1 - k) * (1 - w)) * cube[i_][j_][k_];
      }
    }
  }
  return accum;
}

// TODO: is_arithmetic_v is only for builtin types
// but there is no reason for not supporting matrix like type
//
// Precondition:
//   - x should be st the operation doesn't overflow
// Postcondition:
//   - returns the hermite_cube of x
template <typename T>
  requires std::is_arithmetic_v<T>
constexpr T hermite_cube(T const &x) {
  return x * x * (3 - 2 * x);
}
} // namespace mrl
