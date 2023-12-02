#pragma once

#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "hit_record.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include <optional>

namespace mrl {
template <typename Object, typename Generator>
concept Hittable =
    DoubleGenerator<Generator> &&
    requires(Object const &obj, ray_t const &ray, interval_t const &interval,
             generator_view<Generator> gen) {
      // Postcondition:
      //   - If ray doesn't intersect then return nullopt
      //   - t should be the minimum possible value for which ray intersects
      //   object
      //   - normal should be perpendicular to the hit point
      //   - normal should always point outside of object
      {
        hit(obj, ray, interval, gen)
      } -> std::same_as<std::optional<hit_record_t>>;
    };

template <typename Object, typename Generator>
concept SceneObject = Hittable<Object, Generator>;
} // namespace mrl
