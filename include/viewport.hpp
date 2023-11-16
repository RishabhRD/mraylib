#pragma once

#include "dimension.hpp"

namespace mrl {
struct viewport_t {
  double width;
  double height;
};

constexpr double viewport_width(dimension_t<int> scene_dimensions,
                                double viewport_height) {
  return viewport_height * (static_cast<double>(scene_dimensions.width) /
                            scene_dimensions.height);
}
} // namespace mrl
