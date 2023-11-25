#pragma once

#include "hit_record.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include <algorithm>
#include <bits/ranges_base.h>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>

namespace mrl {
template <std::ranges::input_range SceneObjectRange>
  requires SceneObject<std::ranges::range_value_t<SceneObjectRange>>
constexpr std::optional<hit_record_t> hit(SceneObjectRange const &obj,
                                          ray_t const &ray) {
  namespace rv = std::views;
  auto hit_results =
      obj                                                            //
      | rv::transform([&ray](auto const &e) { return hit(e, ray); }) //
      | rv::filter([](auto const &res) { return res.has_value(); });
  auto min_iter = std::ranges::min_element(hit_results, std::less<>(),
                                           std::mem_fn(&hit_record_t::t));
  if (min_iter == std::ranges::end(hit_results))
    return std::nullopt;
  else
    return *min_iter;
}
} // namespace mrl
