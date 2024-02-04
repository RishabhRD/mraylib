#pragma once

#include "hit_info.hpp"
#include "scene_objects/concepts.hpp"
#include "traits.hpp"
#include <ranges>

namespace mrl {
template <std::ranges::input_range SceneObjectRange>
  requires SceneObject<std::ranges::range_value_t<SceneObjectRange>>
struct hit_object<SceneObjectRange> {
  using type = hit_object_t<std::ranges::range_value_t<SceneObjectRange>>;
};

template <std::ranges::input_range SceneObjectRange>
  requires SceneObject<std::ranges::range_value_t<SceneObjectRange>>
constexpr std::optional<hit_info_t<hit_object_t<SceneObjectRange>>>
hit(SceneObjectRange const &obj, ray_t const &ray, interval_t const &i) {
  namespace rv = std::views;
  auto hit_results =
      obj                                                                   //
      | rv::transform([&ray, &i](auto const &e) { return hit(e, ray, i); }) //
      | rv::filter([](auto const &res) { return res.has_value(); });
  auto dist = [](auto const &hit_info) { return hit_info->hit_distance; };
  auto min_iter = std::ranges::min_element(hit_results, std::less<>(), dist);
  if (min_iter == std::ranges::end(hit_results))
    return std::nullopt;
  else
    return *min_iter;
}

template <std::ranges::input_range BoundedObjectRange>
  requires BoundedObject<std::ranges::range_value_t<BoundedObjectRange>>
constexpr bound_t get_bounds(BoundedObjectRange const &rng) {
  bound_t res;
  for (auto const &obj : rng) {
    auto cur = get_bounds(obj);
    res.x_range.min = std::min(res.x_range.min, cur.x_range.min);
    res.x_range.max = std::max(res.x_range.max, cur.x_range.max);

    res.y_range.min = std::min(res.y_range.min, cur.y_range.min);
    res.y_range.max = std::max(res.y_range.max, cur.y_range.max);

    res.z_range.min = std::min(res.z_range.min, cur.z_range.min);
    res.z_range.max = std::max(res.z_range.max, cur.z_range.max);
  }
  return res;
}
} // namespace mrl
