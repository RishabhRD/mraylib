#pragma once

#include "color.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "generator/random_double_generator.hpp"
#include "point.hpp"
#include "std/algorithm.hpp"
#include "textures/concepts.hpp"
#include "textures/texture_coord.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <random>
#include <ranges>
#include <vector>

namespace mrl {
class perlin_noise {
public:
  template <typename URBG>
  constexpr perlin_noise(URBG random_generator)
      : random_doubles(num_points), perm_x(num_points), perm_y(num_points),
        perm_z(num_points) {
    auto gen_double = std::bind_front(
        mrl::basic_random_double_generator{random_generator}, 0, 1);
    std::generate(std::begin(random_doubles), std::end(random_doubles),
                  gen_double);
    random_permutation(std::begin(perm_x), std::end(perm_x), random_generator);
    random_permutation(std::begin(perm_y), std::end(perm_y), random_generator);
    random_permutation(std::begin(perm_z), std::end(perm_z), random_generator);
  }

  perlin_noise(unsigned long seed) : mrl::perlin_noise(std::mt19937(seed)) {}

  // Postcondition:
  //   - A random noise between [0, 1)
  constexpr double operator()(point3 const &p) const {
    auto u = hermite_cube(p.x - std::floor(p.x));
    auto v = hermite_cube(p.y - std::floor(p.y));
    auto w = hermite_cube(p.z - std::floor(p.z));
    auto i = static_cast<int>(std::floor(p.x));
    auto j = static_cast<int>(std::floor(p.y));
    auto k = static_cast<int>(std::floor(p.z));
    namespace vw = std::views;
    double cube[2][2][2];
    for (auto [di, dj, dk] : vw::cartesian_product(
             vw::iota(0, 2), vw::iota(0, 2), vw::iota(0, 2))) {
      cube[di][dj][dk] =
          random_doubles[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^
                         perm_z[(k + dk) & 255]];
    }
    return tri_interpolate(cube, u, v, w);
  }

private:
  constexpr static std::size_t num_points = 256;
  std::vector<double> random_doubles;
  std::vector<std::size_t> perm_x;
  std::vector<std::size_t> perm_y;
  std::vector<std::size_t> perm_z;
};

template <typename Texture> struct perlin_texture {
  using texture_t = Texture;
  texture_t internal_texture;
  perlin_noise noise;
  double scale;

  constexpr perlin_texture(texture_t texture_, perlin_noise noise_,
                           double scale_)
      : internal_texture(std::move(texture_)), noise(std::move(noise_)),
        scale(scale_) {}
};

template <typename Texture>
perlin_texture(Texture, perlin_noise, double) -> perlin_texture<Texture>;

template <DoubleGenerator Generator, Texture<Generator> texture_t>
color_t texture_color(perlin_texture<texture_t> const &texture,
                      texture_coord_t const coord, point3 const &hit_point,
                      generator_view<Generator> rand) {
  return texture_color(texture.internal_texture, coord, hit_point, rand) *
         texture.noise(texture.scale * hit_point);
}
} // namespace mrl
