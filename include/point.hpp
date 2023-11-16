#pragma once

#include "vector.hpp"
namespace mrl {

using point3 = vec3;

constexpr auto distance(point3 const &a, point3 const &b) {
  return (a - b).length();
}

} // namespace mrl
