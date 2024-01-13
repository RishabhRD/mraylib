#pragma once

#include "bound.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "hit_context.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include "scene_objects/scene_object_range.hpp"
#include "std/hierarchy_tree.hpp"
#include <functional>
#include <ranges>

namespace mrl {
namespace __bvh_details {
constexpr auto get_bounds_obj = lift(get_bounds);
constexpr auto union_bounds_obj = lift(union_bounds);
} // namespace __bvh_details
template <typename Object> class bvh_t {
public:
  using object_type = Object;

private:
  using tree_type = hierarchy_tree<object_type, bound_t,
                                   decltype(__bvh_details::get_bounds_obj),
                                   decltype(__bvh_details::union_bounds_obj)>;

  tree_type tree;

public:
  template <std::ranges::random_access_range Range>
  bvh_t(Range &&rng)
      : tree([&rng] {
          auto bound_x_min = [](auto const &obj) {
            return get_bounds(obj).x_range.min;
          };
          std::ranges::sort(rng, std::less<>{}, bound_x_min);
          return tree_type(std::forward<Range>(rng),
                           __bvh_details::get_bounds_obj,
                           __bvh_details::union_bounds_obj);
        }()) {}

  bound_t bounds() const { return tree.bounds(); }

  template <DoubleGenerator Generator>
  std::optional<hit_context_t> hit_ray(ray_t const &r,
                                       interval_t const &interval,
                                       generator_view<Generator> rand) const {
    std::optional<hit_context_t> res;
    auto hit_obj = [&r, &interval, rand, &res](object_type const &obj) {
      auto hit_rec = hit(obj, r, interval, rand);
      if (hit_rec) {
        if (res) {
          *res = std::min(*res, *hit_rec,
                          [](hit_context_t const &a, hit_context_t const &b) {
                            return a.hit_info.t < b.hit_info.t;
                          });
        } else {
          res = hit_rec;
        }
      }
    };
    tree.for_each_if(
        hit_obj, [&r](bound_t const &bound) { return hit_bounds(r, bound); });
    return res;
  }
};

template <std::ranges::random_access_range Range>
bvh_t(Range &&rng) -> bvh_t<std::ranges::range_value_t<Range>>;

template <BoundedObject Object>
inline bound_t get_bounds(bvh_t<Object> const &bvh) {
  return bvh.bounds();
}

template <DoubleGenerator Generator, SceneObject<Generator> Object>
inline std::optional<hit_context_t>
hit(bvh_t<Object> const &bvh, ray_t const &r, interval_t const &interval,
    generator_view<Generator> rand) {
  return bvh.hit_ray(r, interval, rand);
}
} // namespace mrl
