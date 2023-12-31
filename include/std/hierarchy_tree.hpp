#pragma once

#include "std/btree.hpp"
#include "std/functional.hpp"
#include <iterator>
#include <ranges>
#include <type_traits>
#include <variant>

namespace mrl {
template <typename ValueType, typename BoundType, typename GetBounds,
          typename UnionBounds>
class hierarchy_tree {
public:
  using bound_type = BoundType;
  using value_type = ValueType;

  using data_type = std::variant<bound_type, value_type>;

private:
  using tree_type = btree<data_type>;
  using node = typename tree_type::node;
  using node_ptr = typename tree_type::node_ptr;
  [[no_unique_address]] GetBounds get_bounds;
  [[no_unique_address]] UnionBounds union_bounds;
  tree_type internal_tree;

public:
  template <std::ranges::forward_range Range>
  hierarchy_tree(Range &&rng, GetBounds get_bounds_, UnionBounds union_bounds_)
      : get_bounds(std::move(get_bounds_)),
        union_bounds(std::move(union_bounds_)) {

    if constexpr (std::is_rvalue_reference_v<Range>) {
      internal_tree.root =
          build_tree_move(std::ranges::begin(rng), std::ranges::end(rng));
    } else {
      internal_tree.root =
          build_tree(std::ranges::begin(rng), std::ranges::end(rng));
    }
  }

  template <typename F, typename Predicate>
    requires std::invocable<F, data_type const &> &&
             std::invocable<Predicate, bound_type const &> &&
             std::same_as<std::invoke_result_t<Predicate, bound_type const &>,
                          bool>
  void for_each_if(F &&f, Predicate &&satisfy_bounds) const {
    for_each_if(internal_tree.root, std::forward<F>(f),
                std::forward<Predicate>(satisfy_bounds));
  }

  template <typename F>
    requires std::invocable<F, data_type const &>
  void for_each_if(F &&f) const {
    for_each_if(std::forward<F>(f), always(true));
  }

  bound_type const &bounds() const {
    auto const &data = internal_tree.root->data;
    return std::visit(
        overload{
            [](bound_type const &bound) { return bound; },
            [this](value_type const &value) { return get_bounds(value); },
        },
        data);
  }

private:
  template <std::forward_iterator BeginIter, std::forward_iterator EndIter>
  node_ptr build_tree(BeginIter begin, EndIter end) {
    namespace rng = std::ranges;
    if (begin == end)
      return nullptr;
    if (rng::next(begin) == end) {
      return make_btree_node(data_type{*begin});
    }
    auto const n = rng::distance(begin, end);
    auto const mid = std::next(begin, n / 2);
    auto bound_left = get_bounds(rng::subrange(begin, mid));
    auto bound_right = get_bounds(rng::subrange(mid, end));
    auto bounds = union_bounds(std::move(bound_left), std::move(bound_right));
    auto left = build_tree(begin, mid);
    auto right = build_tree(mid, end);
    return make_btree_node(data_type{std::move(bounds)}, std::move(left),
                           std::move(right));
  }

  // TODO: Some better way to say I want to move without duplicating code
  template <std::forward_iterator BeginIter, std::forward_iterator EndIter>
  node_ptr build_tree_move(BeginIter begin, EndIter end) {
    namespace rng = std::ranges;
    if (begin == end)
      return nullptr;
    if (rng::next(begin) == end) {
      return make_btree_node(data_type{std::move(*begin)});
    }
    auto const n = rng::distance(begin, end);
    auto const mid = std::next(begin, n / 2);
    auto bound_left = get_bounds(rng::subrange(begin, mid));
    auto bound_right = get_bounds(rng::subrange(mid, end));
    auto bounds = union_bounds(std::move(bound_left), std::move(bound_right));
    auto left = build_tree_move(begin, mid);
    auto right = build_tree_move(mid, end);
    return make_btree_node(data_type{std::move(bounds)}, left, right);
  }

  template <typename F, typename Predicate>
  void for_each_if(node_ptr const &root, F f, Predicate pred) const {
    if (root == nullptr)
      return;
    auto const &data = root->data;
    std::visit(
        overload{
            [&root, f, pred, this](bound_type const &bounds) {
              if (pred(bounds)) {
                for_each_if(root->left, f, pred);
                for_each_if(root->right, f, pred);
              }
            },
            [&root, f, pred, this](value_type const &value) { f(value); },
        },
        data);
  }
};
} // namespace mrl
