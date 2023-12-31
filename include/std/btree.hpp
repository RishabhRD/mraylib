#pragma once

#include <memory>

namespace mrl {
namespace __details {
template <typename Data> struct node {
  Data data;
  node *left;
  node *right;
};

template <typename Data>
std::unique_ptr<node<Data>> *clone(std::unique_ptr<node<Data>> const &root) {
  if (root == nullptr)
    return nullptr;
  auto left = clone(root->left);
  auto right = clone(root->right);
  return std::make_unique<node<Data>>(root->data, left, right);
}
} // namespace __details

template <typename Data> struct btree {
  using node = __details::node<Data>;
  using node_ptr = std::unique_ptr<node>;
  std::unique_ptr<node> root{nullptr};
  btree(btree &&other) : root(std::move(other.root)) {}
  btree(btree const &other) : root(__details::clone(other)) {}
  btree &operator=(btree other) {
    std::swap(root, other.root);
    return *this;
  }
  ~btree() {}
};

template <typename Data>
auto make_btree_node(Data data, typename btree<Data>::node_ptr left = nullptr,
                     typename btree<Data>::node_ptr right = nullptr) {
  return std::make_unique<typename btree<Data>::node>(
      std::move(data), std::move(left), std::move(right));
}

} // namespace mrl
