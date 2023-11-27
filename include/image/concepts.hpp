#pragma once

#include "color.hpp"
namespace mrl {

template <typename Image>
concept RandomAccessImage =
    requires(Image &img1, Image const &img2, int x, int y) {
      { img1.at(x, y) } -> std::same_as<color_t &>;
      { img1.width() } -> std::same_as<int>;
      { img1.height() } -> std::same_as<int>;
      { img2.at(x, y) } -> std::same_as<color_t const &>;
      { img2.width() } -> std::same_as<int>;
      { img2.height() } -> std::same_as<int>;
    };
}
