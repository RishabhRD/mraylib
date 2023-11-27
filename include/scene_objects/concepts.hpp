#pragma once

#include "hit_record.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include <optional>

namespace mrl {
template <typename T>
concept Hittable =
    requires(T const &obj, ray_t const &ray, interval_t const &interval) {
      // Postcondition:
      //   - If ray doesn't intersect then return nullopt
      //   - t should be the minimum possible value for which ray intersects
      //   object
      //   - normal of hit record should be in opposite direction of ray
      { hit(obj, ray, interval) } -> std::same_as<std::optional<hit_record_t>>;
    };

template <typename T>
concept SceneObject = Hittable<T>;
} // namespace mrl
