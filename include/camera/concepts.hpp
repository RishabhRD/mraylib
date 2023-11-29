#pragma once

#include "angle.hpp"
#include <concepts>

namespace mrl {
template <typename camera_t>
concept Camera = requires(camera_t const &camera) {
  { focal_length(camera) } -> std::same_as<double>;
  { vertical_fov(camera) } -> std::same_as<angle_t>;
};
}
