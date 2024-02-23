#pragma once

#include "color.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "point.hpp"
#include "scale_2d.hpp"

namespace mrl {
template <typename T, typename Generator>
concept Texture =
    DoubleGenerator<Generator> &&
    requires(T const &texture, scale_2d_t const &coord, point3 const &hit_point,
             generator_view<Generator> rand) {
      {
        texture_color(texture, coord, hit_point, rand)
      } -> std::same_as<color_t>;
    };
} // namespace mrl
