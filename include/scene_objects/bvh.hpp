#pragma once

#include "bound.hpp"
#include "std/hierarchy_tree.hpp"
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
  template <std::ranges::forward_range Range>
  bvh_t(Range &&rng)
      : tree(std::forward<Range>(rng), __bvh_details::get_bounds_obj,
             __bvh_details::union_bounds_obj) {}

  bound_t get_bounds() const { return tree.bounds(); }
};
} // namespace mrl
