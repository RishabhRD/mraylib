#pragma once

#include "vector.hpp"
#include <ostream>

namespace mrl {

// NOTE: this is there to solve scoping issue... C++ things
class direction_t;
constexpr direction_t dir_from_unit(vec3 vec);

class direction_t {
  vec3 direction_;

  constexpr direction_t() = default;

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

  // Precondition:
  //   - vec should be a unit vector
  //
  // Postcondition:
  //   - Guarantees that no call to std::sqrt is made in construction
  friend constexpr direction_t dir_from_unit(vec3 vec) {
    direction_t dir;
    dir.direction_ = vec;
    return dir;
  }
};
} // namespace mrl
