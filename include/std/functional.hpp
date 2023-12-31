#pragma once

#include <utility>
namespace mrl {
template <class... Ts> struct overload : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overload(Ts...) -> overload<Ts...>;

constexpr auto always(auto val) {
  return [v = std::move(val)](auto &&...) { return v; };
}
} // namespace mrl
