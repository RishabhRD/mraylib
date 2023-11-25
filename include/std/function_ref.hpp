#pragma once

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>
namespace mrl {
template <typename F> class function_ref {
private:
  F *f;

public:
  function_ref(F &f_) : f(&f_) {}

  template <typename... Args> auto operator()(Args &&...args) const {
    if constexpr (std::same_as<std::invoke_result_t<F, Args...>, void>) {
      std::invoke(*f, std::forward<Args>(args)...);
    } else {
      return std::invoke(*f, std::forward<Args>(args)...);
    }
  }
};

template <typename F> function_ref(F &f) -> function_ref<F>;
} // namespace mrl
