#pragma once

#include "direction.hpp"
#include "point.hpp"
#include <concepts>

namespace mrl {

struct ray_t {
  point3 origin;
  direction_t direction;

  constexpr point3 at(double t) const { return origin + t * direction.val(); }
};

} // namespace mrl
