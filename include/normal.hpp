#pragma once

#include "direction.hpp"

namespace mrl {
constexpr bool is_normal_away_from_ray(direction_t normal,
                                       direction_t ray_dir) {
  return dot(normal.val(), ray_dir.val()) <= 0;
}

// Postcondition:
//   - Returns the normal in opposite direction of ray_dir
constexpr direction_t normal_dir(direction_t normal, direction_t ray_dir) {
  return is_normal_away_from_ray(normal, ray_dir) ? normal : opposite(normal);
}

} // namespace mrl
