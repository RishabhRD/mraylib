#pragma once

#include "vector.hpp"
#include <ostream>

namespace mrl {
class direction_t {
  vec3 direction_;

public:
  // Precondition:
  //   - vec3(x, y, z) is a unit vector
  //
  // Postcondition:
  //   - No normalization would be applied
  constexpr direction_t(double x, double y, double z)
      : direction_(vec3{x, y, z}) {}

  // Postcondition:
  //   - Normalization would be applied
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
// Precondition:
//   - vec should be a unit vector
//
// Postcondition:
//   - Guarantees that no call to std::sqrt is made in construction
constexpr direction_t dir_from_unit(vec3 vec) {
  return direction_t{vec.x, vec.y, vec.z};
}

constexpr direction_t opposite(direction_t dir) {
  return dir_from_unit(-dir.val());
}
} // namespace mrl
