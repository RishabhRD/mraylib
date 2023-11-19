#pragma once

#include "hit_record.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <vector>

namespace mrl {
template <Hittable T, typename Allocator = std::allocator<T>>
using scene_object_list = std::vector<T, Allocator>;

template <Hittable T, typename Allocator>
std::optional<hit_record_t> hit(scene_object_list<T, Allocator> const &obj,
                                ray_t const &ray) {
  namespace rv = std::views;
  auto hit_results =
      obj                                                            //
      | rv::transform([&ray](auto const &e) { return hit(e, ray); }) //
      | rv::filter([](auto const &res) { return res.has_value(); });
  auto min_iter = std::ranges::min_element(hit_results, std::less<>(),
                                           std::mem_fn(&hit_record_t::ray_t));
  if (min_iter == std::ranges::end(hit_results))
    return std::nullopt;
  else
    return *min_iter;
}
} // namespace mrl
