#pragma once

#include "interval.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "scale_2d.hpp"
#include <optional>

namespace mrl {
template <typename shape_t>
// Precondition:
//   - p should lie on surface of shape
concept Shape = requires(shape_t const &shape, ray_t const &r, point3 const &p,
                         interval_t const &i) {
  // Postconditon:
  //   - normal points to outside the object
  { normal_at(shape, p) } -> std::same_as<direction_t>;
  { scaling_2d_at(shape, p) } -> std::same_as<scale_2d_t>;
  { ray_hit_distance(shape, r, i) } -> std::same_as<std::optional<double>>;
};
}
