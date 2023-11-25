#pragma once

#define lift(func)                                                             \
  [](auto &&...__args__) noexcept(                                             \
      noexcept(func(std::forward<decltype(__args__)>(__args__)...)))           \
      -> decltype(func(std::forward<decltype(__args__)>(__args__)...)) {       \
    return func(std::forward<decltype(__args__)>(__args__)...);                \
  }
