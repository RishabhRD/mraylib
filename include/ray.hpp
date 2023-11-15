#pragma once

#include "point.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <concepts>

namespace mrl {

template <std::regular T> struct ray {
  using component_type = T;

  point3_basic<T> origin;
  vec3_basic<T> direction;

  constexpr point3_basic<T> at(double t) const {
    return origin + t * direction;
  }
};

} // namespace mrl
