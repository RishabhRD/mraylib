#pragma once

#include "hit_record.hpp"
#include "point.hpp"
#include "ray.hpp"
#include <optional>

namespace mrl {
template <typename T>
concept Hittable = requires(T const &h, ray_t const &ray, point3 const &point) {
  // Postcondition:
  //   - If ray doesn't intersect then return nullopt
  //   - t should be the minimum possible value for which ray intersects object
  //   - normal of hit record should be in opposite direction of ray
  { hit(h, ray) } -> std::same_as<std::optional<hit_record_t>>;
};

template <typename T>
concept SceneObject = Hittable<T>;
} // namespace mrl
