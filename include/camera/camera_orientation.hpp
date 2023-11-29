#pragma once

#include "direction.hpp"
#include "point.hpp"

namespace mrl {
struct camera_orientation_t {
  point3 position;
  direction_t direction;
};

} // namespace mrl
