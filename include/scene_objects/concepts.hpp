#pragma once

#include "bound.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "interval.hpp"
#include "materials/emit_info.hpp"
#include "materials/scatter_info.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "scale_2d.hpp"
#include "scene_objects/traits.hpp"

namespace mrl {
template <typename Object, typename Generator>
concept HitObject =
    DoubleGenerator<Generator> &&
    // Precondition:
    //   - p is a point on surface of obj
    requires(Object const &obj, generator_view<Generator> rand, point3 const &p,
             ray_t const &r, double hit_distance) {
      { normal_at(obj, p) } -> std::same_as<direction_t>;
      { scaling_2d_at(obj, p) } -> std::same_as<scale_2d_t>;
      {
        scattering_for(obj, r, hit_distance, rand)
      } -> std::same_as<std::optional<scatter_info_t>>;
      { emission_at(obj, p, rand) } -> std::same_as<std::optional<emit_info_t>>;
    };

template <typename Object>
concept Hittable =
    requires(Object const &obj, ray_t const &ray, interval_t const &interval) {
      typename hit_object_t<Object>;
      {
        hit(obj, ray, interval)
      } -> std::same_as<std::optional<hit_info_of<Object>>>;
    };

template <typename Object>
concept SceneObject = Hittable<Object>;

template <typename Object>
concept BoundedObject = SceneObject<Object> && requires(Object const &obj) {
  { get_bounds(obj) } -> std::same_as<bound_t>;
};
} // namespace mrl
