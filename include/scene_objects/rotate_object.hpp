#pragma once

#include "angle.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "hit_context.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "rotation.hpp"
#include "scene_objects/concepts.hpp"

namespace mrl {
template <typename Object> struct rotate_object {
  using object_type = Object;

  object_type internal_obj;
  ray_t axis_of_rotation;
  angle_t angle_of_rotation;

  constexpr rotate_object(object_type obj, ray_t axis, angle_t angle)
      : internal_obj(std::move(obj)), axis_of_rotation(axis),
        angle_of_rotation(angle) {}
};

template <typename Object>
rotate_object(Object, ray_t, angle_t) -> rotate_object<Object>;

template <DoubleGenerator Generator, Hittable<Generator> Object>
constexpr std::optional<hit_context_t> hit(rotate_object<Object> const &obj,
                                           ray_t r, interval_t const &interval,
                                           generator_view<Generator> rand) {
  r = rotate(r, obj.axis_of_rotation, -obj.angle_of_rotation);
  auto res = hit(obj.internal_obj, r, interval, rand);
  if (!res)
    return std::nullopt;
  auto hit_ray =
      rotate(ray_t{res->hit_info.hit_point, res->hit_info.outward_normal},
             obj.axis_of_rotation, obj.angle_of_rotation);
  res->hit_info.hit_point = hit_ray.origin;
  res->hit_info.outward_normal = hit_ray.direction;
  if (res->scatter_info) {
    res->scatter_info->scattered_ray =
        rotate(res->scatter_info->scattered_ray, obj.axis_of_rotation,
               obj.angle_of_rotation);
  }
  return res;
}

template <BoundedObject Object>
constexpr bound_t get_bounds(rotate_object<Object> const &obj) {
  return rotate(get_bounds(obj.internal_obj), obj.axis_of_rotation,
                obj.angle_of_rotation);
}
} // namespace mrl
