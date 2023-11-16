#pragma once

#include "aspect_ratio.hpp"
#include <algorithm>

namespace mrl {
constexpr int scene_height(aspect_ratio ratio, int width) {
  return std::min(1, static_cast<int>(width / ratio.val()));
}
} // namespace mrl
