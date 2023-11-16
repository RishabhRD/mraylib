#pragma once

#include "direction.hpp"
#include "point.hpp"
#include "vector.hpp"

namespace mrl {
struct camera_orientation_t {
  point3 position;
  direction_t direction;
};

} // namespace mrl
