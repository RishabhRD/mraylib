#pragma once

#include "vector.hpp"
#include "viewport.hpp"

namespace mrl {
struct camera_t {
  viewport_t viewport;
  double focal_length;
};

} // namespace mrl
