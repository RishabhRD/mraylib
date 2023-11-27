#pragma once

#include "direction.hpp"
#include "ray.hpp"
#include "vector.hpp"
namespace mrl {
constexpr direction_t reflection_dir(direction_t const &ray_dir,
                                     direction_t const &normal) {
  return dir_from_unit(ray_dir.val() -
                       2 * dot(ray_dir.val(), normal.val()) * normal.val());
}
} // namespace mrl
