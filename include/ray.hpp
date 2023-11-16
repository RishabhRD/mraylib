#pragma once

#include "direction.hpp"
#include "point.hpp"
#include <concepts>
#include <ostream>

namespace mrl {

struct ray_t {
  point3 origin;
  direction_t direction;

  constexpr point3 at(double t) const { return origin + t * direction.val(); }

  friend std::ostream &operator<<(std::ostream &os, ray_t const &dir) {
    os << "{ origin : " << dir.origin << " , direction : " << dir.direction
       << " }";
    return os;
  }
};

} // namespace mrl
