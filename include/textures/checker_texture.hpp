#pragma once

#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "point.hpp"
#include "scale_2d.hpp"
#include "textures/concepts.hpp"
#include <utility>
namespace mrl {
template <typename EvenTexture, typename OddTexture> struct checker_texture {
  using even_texture_t = EvenTexture;
  using odd_texture_t = OddTexture;

  double inv_scale;
  even_texture_t even_texture;
  odd_texture_t odd_texture;

  checker_texture(double scale, even_texture_t even_texture_,
                  odd_texture_t odd_texture_)
      : inv_scale{1.0 / scale}, even_texture{std::move(even_texture_)},
        odd_texture{std::move(odd_texture_)} {}
};

template <typename EvenTexture, typename OddTexture>
checker_texture(double, EvenTexture, OddTexture)
    -> checker_texture<EvenTexture, OddTexture>;

template <DoubleGenerator Generator, Texture<Generator> EvenTexture,
          Texture<Generator> OddTexture>
constexpr color_t
texture_color(checker_texture<EvenTexture, OddTexture> const &texture,
              scale_2d_t const &coord, point3 const &hit_point,
              generator_view<Generator> rand) {
  auto x = static_cast<int>(std::floor(texture.inv_scale * hit_point.x));
  auto y = static_cast<int>(std::floor(texture.inv_scale * hit_point.y));
  auto z = static_cast<int>(std::floor(texture.inv_scale * hit_point.z));

  bool is_even = (x + y + z) % 2 == 0;

  return is_even ? texture_color(texture.even_texture, coord, hit_point, rand)
                 : texture_color(texture.odd_texture, coord, hit_point, rand);
}
} // namespace mrl
