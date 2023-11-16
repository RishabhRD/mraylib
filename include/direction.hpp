#pragma once

#include "vector.hpp"
#include <ostream>

namespace mrl {
class direction_t {
  vec3 direction_;

public:
  constexpr direction_t(vec3 direction)
      : direction_(mrl::normalize(direction)) {}

  constexpr vec3 val() const { return direction_; }

  constexpr direction_t &operator=(vec3 const &v) {
    direction_ = mrl::normalize(v);
    return *this;
  }

  constexpr direction_t &operator=(direction_t const &d) = default;
  constexpr direction_t &operator=(direction_t &&d) = default;
  constexpr direction_t(direction_t const &d) = default;
  constexpr direction_t(direction_t &&d) = default;

  friend std::ostream &operator<<(std::ostream &os, direction_t const &dir) {
    return os << dir.val();
  }
};
} // namespace mrl
