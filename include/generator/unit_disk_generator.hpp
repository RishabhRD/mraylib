#pragma once
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "vector.hpp"

namespace mrl {
struct unit_disk_generator {
  template <DoubleGenerator ComponentGenerator>
  constexpr vec3 operator()(generator_view<ComponentGenerator> gen_component) {
    while (true) {
      auto p = vec3(gen_component(-1.0, 1.0), gen_component(-1.0, 1.0), 0);
      if (p.length_square() < 1)
        return p;
    }
  }
};
} // namespace mrl
