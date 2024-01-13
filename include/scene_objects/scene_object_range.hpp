#pragma once

#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "hit_context.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include <algorithm>
#include <bits/ranges_base.h>
#include <functional>
#include <optional>
#include <ranges>

namespace mrl {
template <DoubleGenerator Generator, std::ranges::input_range SceneObjectRange>
  requires SceneObject<std::ranges::range_value_t<SceneObjectRange>, Generator>
constexpr std::optional<hit_context_t>
hit(SceneObjectRange const &obj, ray_t const &ray, interval_t const &t_rng,
    generator_view<Generator> rand) {
  namespace rv = std::views;
  auto hit_results =
      obj //
      | rv::transform([&ray, &t_rng, rand](auto const &e) {
          return hit(e, ray, t_rng, rand);
        }) //
      | rv::filter([](auto const &res) { return res.has_value(); });
  auto get_t = [](auto const &ctx) { return ctx->hit_info.t; };
  auto min_iter = std::ranges::min_element(hit_results, std::less<>(), get_t);
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
