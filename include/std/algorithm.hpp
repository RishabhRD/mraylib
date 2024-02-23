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
