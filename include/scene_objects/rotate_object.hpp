#pragma once

#include "angle.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "hit_info.hpp"
#include "interval.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "rotation.hpp"
#include "scene_objects/concepts.hpp"
#include "scene_objects/traits.hpp"

namespace mrl {
template <typename Object> struct rotate_hit_object {
  hit_object_t<Object> hit_obj;
  ray_t axis_of_rotation;
  angle_t angle_of_rotation;
};

template <typename Object> struct rotate_object {
  using object_type = Object;
  using hit_object_type = rotate_hit_object<Object>;

  object_type internal_obj;
  ray_t axis_of_rotation;
  angle_t angle_of_rotation;

  constexpr rotate_object(object_type obj, ray_t axis, angle_t angle)
      : internal_obj(std::move(obj)), axis_of_rotation(axis),
        angle_of_rotation(angle) {}
};

template <typename Object>
rotate_object(Object, ray_t, angle_t) -> rotate_object<Object>;

template <typename Object>
constexpr auto scaling_2d_at(rotate_hit_object<Object> const &o,
                             point3 const &p) {
  return scaling_2d_at(o.hit_obj,
                       rotate(p, o.axis_of_rotation, -o.angle_of_rotation));
}

template <DoubleGenerator Generator, SceneObject Object>
constexpr auto scattering_for(rotate_hit_object<Object> const &o,
                              ray_t const &r, double hit_distance,
                              generator_view<Generator> rand) {
  return scattering_for(o.hit_obj,
                        rotate(r, o.axis_of_rotation, -o.angle_of_rotation),
                        hit_distance, rand);
}

template <DoubleGenerator Generator, SceneObject Object>
constexpr auto emission_at(rotate_hit_object<Object> const &o, point3 const &p,
                           generator_view<Generator> rand) {
  return emission_at(o.hit_obj,
                     rotate(p, o.axis_of_rotation, -o.angle_of_rotation), rand);
}

template <SceneObject Object>
constexpr auto normal_at(rotate_hit_object<Object> const &o, point3 const &p) {
  auto n =
      normal_at(o.hit_obj, rotate(p, o.axis_of_rotation, -o.angle_of_rotation))
          .val();
  n = rotate(n, o.axis_of_rotation, o.angle_of_rotation);
  return direction_t{n.x, n.y, n.z};
}

template <SceneObject Object>
constexpr std::optional<hit_info_t<rotate_hit_object<Object>>>
hit(rotate_object<Object> const &obj, ray_t r, interval_t const &interval) {
  r = rotate(r, obj.axis_of_rotation, -obj.angle_of_rotation);
  auto internal_hit = hit(obj.internal_obj, r, interval);
  if (!internal_hit)
    return std::nullopt;
  return hit_info_t<hit_object_t<rotate_object<Object>>>{
      internal_hit->hit_distance,
      {std::move(internal_hit->hit_object), obj.axis_of_rotation,
       obj.angle_of_rotation}};
}

template <BoundedObject Object>
constexpr bound_t get_bounds(rotate_object<Object> const &obj) {
  return rotate(get_bounds(obj.internal_obj), obj.axis_of_rotation,
                obj.angle_of_rotation);
}
} // namespace mrl
