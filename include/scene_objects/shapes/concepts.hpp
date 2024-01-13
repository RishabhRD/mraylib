#pragma once

#include "hit_info.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
namespace mrl {
template <typename shape_t>
concept Shape =
    BoundedObject<shape_t> && requires(shape_t const &shape, ray_t const &ray,
                                       interval_t const &interval) {
      { hit(shape, ray, interval) } -> std::same_as<std::optional<hit_info_t>>;
    };
}
