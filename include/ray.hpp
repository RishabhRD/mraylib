#pragma once

#include "point.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <concepts>

namespace mrl {

struct ray_t {
  point3 origin;
  vec3 direction;

  constexpr point3 at(double t) const { return origin + t * direction; }
};

} // namespace mrl
