#pragma once

#include "camera.hpp"
#include "camera_orientation.hpp"

namespace mrl {
struct scene_viewer_t {
  camera_t camera;
  camera_orientation_t camera_orientation;
};
} // namespace mrl
