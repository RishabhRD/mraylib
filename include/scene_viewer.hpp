#pragma once

#include "camera.hpp"
#include "camera_orientation.hpp"

namespace mrl {
struct scene_viewer {
  camera camera;
  camera_orientation camera_orientation;
};
} // namespace mrl
