#pragma once

#include "aspect_ratio.hpp"
#include "dimension.hpp"
#include <algorithm>
namespace mrl {
constexpr int scene_height(aspect_ratio ratio, int width) {
  return std::min(1, static_cast<int>(width / ratio.val()));
}

constexpr double viewport_width(dimension<int> scene_dimensions,
                                double viewport_height) {
  return viewport_height * (static_cast<double>(scene_dimensions.width) /
                            scene_dimensions.height);
}
} // namespace mrl
