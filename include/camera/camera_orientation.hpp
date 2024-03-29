#pragma once

#include "direction.hpp"
#include "point.hpp"

namespace mrl {
struct camera_orientation_t {
  point3 look_from;
  point3 look_at;
  // Precondition: up_dir is perpendicular to (look_at - look_from)
  direction_t up_dir;
};

// Precondition:
//   - dot(vec, camera_point_dir) != 0
//
// Postcondition:
//   - returned direction is perpendicular to camera_point_dir
constexpr direction_t calc_up_dir(vec3 vec, direction_t camera_point_dir) {
  auto right = cross(camera_point_dir.val(), vec);
  return cross(right, camera_point_dir.val());
}

// Postcondition: returned direction is perpendicular to camera_point_dir
constexpr direction_t random_up_dir(direction_t camera_point_dir) {
  vec3 dir = camera_point_dir.val();
  ++dir.x;
  return calc_up_dir(dir, camera_point_dir);
}

} // namespace mrl
