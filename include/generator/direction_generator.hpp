#pragma once

#include "direction.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "vector.hpp"

namespace mrl {
struct direction_generator {
  template <DoubleGenerator ComponentGenerator>
  constexpr direction_t operator()(generator_view<ComponentGenerator> gen) {
    return direction_t{vec3{gen(-1.0, 1.0), gen(-1.0, 1.0), gen(-1.0, 1.0)}};
  }
};
} // namespace mrl
