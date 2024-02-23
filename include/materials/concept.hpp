#pragma once

#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "materials/emit_info.hpp"
#include "materials/material_context.hpp"
#include "materials/scatter_info.hpp"
#include <optional>

namespace mrl {
template <typename T, typename Generator>
concept LightScatterer =
    DoubleGenerator<Generator> &&
    requires(T const &material, scattering_context const &ctx,
             generator_view<Generator> rand) {
      {
        scatter(material, ctx, rand)
      } -> std::same_as<std::optional<scatter_info_t>>;
    };

template <typename T, typename Generator>
concept LightEmitter = DoubleGenerator<Generator> &&
                       requires(T const &light, emission_context const ctx,
                                generator_view<Generator> rand) {
                         {
                           emit(light, ctx, rand)
                         } -> std::same_as<std::optional<emit_info_t>>;
                       };
} // namespace mrl
