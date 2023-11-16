#pragma once

#include "color.hpp"
#include <concepts>
#include <type_traits>
namespace mrl {

template <typename Image>
concept RandomAccessImage = requires(Image &x, Image const &y) {
  { x.at(std::declval<int>(), std::declval<int>()) } -> std::same_as<color_t &>;

  {
    y.at(std::declval<int>(), std::declval<int>())
  } -> std::same_as<color_t const &>;

  { x.width() } -> std::same_as<int>;
  { x.height() } -> std::same_as<int>;
  { y.width() } -> std::same_as<int>;
  { y.height() } -> std::same_as<int>;
};

}
