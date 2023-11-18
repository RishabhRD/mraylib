#pragma once

#include "direction.hpp"
#include "point.hpp"
namespace mrl {
struct hit_record_t {
  double ray_t;
  point3 hit_point;
  direction_t normal;
};
} // namespace mrl
