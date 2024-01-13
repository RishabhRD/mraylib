#pragma once

#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "hit_info.hpp"
#include "materials/emit_info.hpp"
#include "materials/scatter_info.hpp"
#include "ray.hpp"
#include <optional>

namespace mrl {
template <typename T, typename Generator>
concept LightScatterer =
    DoubleGenerator<Generator> &&
    requires(T const &material, ray_t const &ray, hit_info_t const &hit_info,
             generator_view<Generator> rand) {
      {
        scatter(material, ray, hit_info, rand)
      } -> std::same_as<std::optional<scatter_info_t>>;
    };

template <typename T, typename Generator>
concept LightEmitter = DoubleGenerator<Generator> &&
                       requires(T const &light, hit_info_t const &hit_info,
                                generator_view<Generator> rand) {
                         {
                           emit(light, hit_info, rand)
                         } -> std::same_as<std::optional<emit_info_t>>;
                       };
} // namespace mrl
