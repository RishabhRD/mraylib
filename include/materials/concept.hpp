#pragma once

#include "materials/scatter_record.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <optional>

namespace mrl {
template <typename T>
concept Material = requires(T &material, ray_t const &ray,
                            point3 const &hit_point, vec3 const &normal) {
  // Precondition:
  //   - normal should always point outside of object
  {
    scatter(material, ray, hit_point, normal)
  } -> std::same_as<std::optional<scatter_record_t>>;
};
} // namespace mrl
