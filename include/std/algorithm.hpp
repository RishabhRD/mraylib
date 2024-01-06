#pragma once

#include <algorithm>
#include <iterator>
#include <numeric>
namespace mrl {
// Postcondition:
//   - [begin, end) would contain a random permutation from [0, end - begin)
template <std::random_access_iterator Iter, typename Generator>
constexpr void random_permutation(Iter begin, Iter end, Generator &&gen) {
  using value_type = typename std::iterator_traits<Iter>::value_type;
  std::iota(begin, end, value_type{});
  std::shuffle(begin, end, gen);
}
} // namespace mrl
