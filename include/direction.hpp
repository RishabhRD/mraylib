#pragma once

#include "vector.hpp"

namespace mrl {
class direction_t {
  vec3 direction_;

public:
  constexpr direction_t(vec3 direction)
      : direction_(mrl::unit_vector(direction)) {}

  constexpr vec3 val() const { return direction_; }

  constexpr direction_t &operator=(vec3 const &v) {
    direction_ = mrl::unit_vector(v);
    return *this;
  }

  constexpr direction_t &operator=(direction_t const &d) = default;
  constexpr direction_t &operator=(direction_t &&d) = default;
  constexpr direction_t(direction_t const &d) = default;
  constexpr direction_t(direction_t &&d) = default;
};
} // namespace mrl
