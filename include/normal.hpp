#pragma once

#include "direction.hpp"

namespace mrl {
// Postcondition:
//   - Returns the normal in opposite direction of ray_dir
constexpr direction_t normal_dir(direction_t normal, direction_t ray_dir) {
  return dot(normal.val(), ray_dir.val()) <= 0 ? normal
                                               : dir_from_unit(-normal.val());
}
} // namespace mrl
