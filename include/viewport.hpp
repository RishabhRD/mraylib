#pragma once

#include "scene.hpp"

namespace mrl {
struct viewport_t {
  double width;
  double height;
};

constexpr double viewport_width(scene_t const &scene, double viewport_height) {
  return viewport_height * (static_cast<double>(scene.width) / scene.height);
}

constexpr viewport_t make_viewport(scene_t const &scene,
                                   double viewport_height) {
  return {
      .width = viewport_width(scene, viewport_height),
      .height = viewport_height,
  };
}
} // namespace mrl
