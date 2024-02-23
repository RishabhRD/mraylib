#pragma once

#include <utility>

#define lift(func)                                                             \
  [](auto &&...__args__) noexcept(                                             \
      noexcept(func(std::forward<decltype(__args__)>(__args__)...)))           \
      -> decltype(func(std::forward<decltype(__args__)>(__args__)...)) {       \
    return func(std::forward<decltype(__args__)>(__args__)...);                \
  }
namespace mrl {
template <class... Ts> struct overload : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overload(Ts...) -> overload<Ts...>;

constexpr auto always(auto val) {
  return [v = std::move(val)](auto &&...) { return v; };
}
} // namespace mrl
