#pragma once

#include "direction.hpp"
#include "point.hpp"

namespace mrl {
struct camera_orientation_t {
  point3 position;
  direction_t direction;
  // Precondition: up_dir is perpendicular to direction
  direction_t up_dir;
};

// Postcondition: returned direction is perpendicular to camera_point_dir
constexpr direction_t random_up_dir(direction_t camera_point_dir) {
  vec3 dir = camera_point_dir.val();
  ++dir.x;
  auto right = cross(camera_point_dir.val(), dir);
  return cross(right, camera_point_dir.val());
}

} // namespace mrl
