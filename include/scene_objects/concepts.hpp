#pragma once

#include "direction.hpp"
#include "hit_record.hpp"
#include "point.hpp"
#include "ray.hpp"
#include <concepts>
#include <optional>

namespace mrl {
template <typename T>
concept Hittable = requires(T const &h, ray_t const &ray, point3 const &point) {
  // Postcondition:
  //   - If ray doesn't intersect then return nullopt
  //   - t should be the minimum possible value for which ray intersects object
  { hit(h, ray) } -> std::same_as<std::optional<hit_record_t>>;
};
}
