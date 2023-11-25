#pragma once

#include "point.hpp"
namespace mrl {
struct sampler_args_t {
  point3 point;
  vec3 pixel_delta_u;
  vec3 pixel_delta_v;
};
} // namespace mrl
