#pragma once

#include "point.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <concepts>

namespace mrl {

template <std::regular T> struct ray {
  using component_type = T;

  point3 origin;
  vec3 direction;

  constexpr point3 at(double t) const { return origin + t * direction; }
};

} // namespace mrl
