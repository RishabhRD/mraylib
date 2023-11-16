#pragma once

#include "aspect_ratio.hpp"
#include <algorithm>

namespace mrl {
// Postcondition: minimum height is 1.0
constexpr int scene_height(aspect_ratio_t ratio, int width) {
  return std::max(1, static_cast<int>(width / ratio.val()));
}

struct scene_t {
  // Precondition: width, height > 0
  int width;
  int height;
};

constexpr scene_t make_scene(aspect_ratio_t ratio, int width) {
  return {
      width,
      scene_height(ratio, width),
  };
}
} // namespace mrl
