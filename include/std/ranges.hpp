#pragma once

#include <functional>
#include <ranges>
#include <type_traits>

namespace mrl {
template <typename Range, typename V>
concept RangeValueType = requires(Range range) {
  { typename std::ranges::range_value_t<Range>{} } -> std::same_as<V>;
};

namespace views {
namespace __details {
template <std::invocable Function> struct repeat_fn_lambda_t {
  mutable Function fn;

  constexpr repeat_fn_lambda_t(Function f) : fn(std::move(f)) {}

  constexpr auto operator()(auto const &) const { return std::invoke(fn); }
};

struct repeat_fn_t {
  template <std::invocable Function>
  constexpr auto operator()(Function f) const {
    namespace vw = std::views;
    return vw::iota(0u) //
           | vw::transform(repeat_fn_lambda_t<Function>{std::move(f)});
  }
};
} // namespace __details

constexpr auto repeat_fn = __details::repeat_fn_t{};

// Precondition: size(rng) is even
template <std::ranges::input_range Range>
constexpr auto chunk_pair(Range &&rng) {
  return std::views::transform(std::views::chunk(rng, 2), [](auto &&inner_rng) {
    auto itr = std::ranges::begin(inner_rng);
    auto first = *itr;
    ++itr;
    auto second = *itr;
    return std::pair{std::move(first), std::move(second)};
  });
}
} // namespace views
namespace ranges {
template <std::ranges::input_range Range> auto to_vector(Range &&rng) {
  using value_type = std::ranges::range_value_t<Range>;
  std::vector<value_type> res;
  for (auto n : rng) {
    res.push_back(std::move(n));
  }
  return res;
}
}; // namespace ranges
} // namespace mrl
