#pragma once

#include "color.hpp"
#include "generator/concepts.hpp"
#include "generator/direction_generator.hpp"
#include "generator/generator_view.hpp"
#include "generator/random_double_generator.hpp"
#include "point.hpp"
#include "std/algorithm.hpp"
#include "textures/concepts.hpp"
#include "textures/texture_coord.hpp"
#include "vector.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <ranges>
#include <vector>

namespace mrl {
// Postcondition:
//   - Returns perlin interpolation of (u, v, w) in c
constexpr double perlin_interpolate(vec3 c[2][2][2], double u, double v,
                                    double w) {
  auto uu = hermite_cube(u);
  auto vv = hermite_cube(v);
  auto ww = hermite_cube(w);
  auto accum = 0.0;

  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      for (int k = 0; k < 2; k++) {
        vec3 weight_v(u - i, v - j, w - k);
        accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) *
                 (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
      }

  return accum;
}

class perlin_noise {
public:
  template <typename URBG>
  constexpr perlin_noise(URBG random_generator)
      : random_vec(num_points), perm_x(num_points), perm_y(num_points),
        perm_z(num_points) {

    auto gen_double = mrl::basic_random_double_generator{random_generator};
    auto gen_vec = [&gen_double] {
      return direction_generator{}(generator_view{gen_double}).val();
    };
    std::generate(std::begin(random_vec), std::end(random_vec), gen_vec);
    random_permutation(std::begin(perm_x), std::end(perm_x), random_generator);
    random_permutation(std::begin(perm_y), std::end(perm_y), random_generator);
    random_permutation(std::begin(perm_z), std::end(perm_z), random_generator);
  }

  perlin_noise(unsigned long seed) : mrl::perlin_noise(std::mt19937(seed)) {}

  // Postcondition:
  //   - A random noise between [-1, 1)
  constexpr double noise_at(point3 const &p) const {
    auto u = p.x - std::floor(p.x);
    auto v = p.y - std::floor(p.y);
    auto w = p.z - std::floor(p.z);
    auto i = static_cast<int>(std::floor(p.x));
    auto j = static_cast<int>(std::floor(p.y));
    auto k = static_cast<int>(std::floor(p.z));
    namespace vw = std::views;
    vec3 cube[2][2][2];
    for (auto [di, dj, dk] : vw::cartesian_product(
             vw::iota(0, 2), vw::iota(0, 2), vw::iota(0, 2))) {
      cube[di][dj][dk] =
          random_vec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^
                     perm_z[(k + dk) & 255]];
    }
    return perlin_interpolate(cube, u, v, w);
  }

  // Postcondition:
  //   - A random noise between [-1, 1)
  constexpr double turbulance_at(point3 const &p, int depth = 7) const {
    auto accum = 0.0;
    auto temp_p = p;
    auto weight = 1.0;

    for (int i = 0; i < depth; i++) {
      accum += weight * noise_at(temp_p);
      weight *= 0.5;
      temp_p *= 2;
    }

    return std::fabs(accum);
  }

private:
  constexpr static std::size_t num_points = 256;
  std::vector<vec3> random_vec;
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
  auto s = texture.scale * hit_point;
  return texture_color(texture.internal_texture, coord, hit_point, rand) * 0.5 *
         (1 + std::sin(s.z + 10 * texture.noise.turbulance_at(s)));
}
} // namespace mrl
