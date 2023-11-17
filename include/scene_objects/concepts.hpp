#pragma once

#include "direction.hpp"
#include "point.hpp"
#include "ray.hpp"
#include <concepts>
#include <optional>

namespace mrl {
template <typename T>
concept Hittable = requires(T const &h, ray_t const &ray, point3 const &point) {
  { hit(h, ray) } -> std::same_as<std::optional<double>>;
  { normal(h, point) } -> std::same_as<direction_t>;
};
}
