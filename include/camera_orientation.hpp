#pragma once

#include "point.hpp"
#include "vector.hpp"

namespace mrl {
struct camera_orientation {
  point3 position;
  vec3 direction;
};
} // namespace mrl
