#pragma once

#include "materials/scatter_record.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <optional>

namespace mrl {
namespace __details {
template <typename T>
concept MaterialNonConst =
    requires(T &material, ray_t const &ray, point3 const &hit_point,
             vec3 const &normal) {
      // Precondition:
      //   - normal should always point outside of object
      {
        scatter(material, ray, hit_point, normal)
      } -> std::same_as<std::optional<scatter_record_t>>;
    };

template <typename T>
concept MaterialConst = requires(T &material, ray_t const &ray,
                                 point3 const &hit_point, vec3 const &normal) {
  // Precondition:
  //   - normal should always point outside of object
  {
    scatter(material, ray, hit_point, normal)
  } -> std::same_as<std::optional<scatter_record_t>>;
};
} // namespace __details

// Precondition:
//   - normal should always point outside of object
template <typename T>
concept Material =
    __details::MaterialNonConst<T> || __details::MaterialConst<T>;
} // namespace mrl
