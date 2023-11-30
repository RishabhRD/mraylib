#pragma once
#include "std/random_double_generator.hpp"
#include "vector.hpp"
#include <functional>

namespace mrl {
template <std::invocable ComponentGenerator> struct basic_unit_disk_generator {
  ComponentGenerator gen_component;

  basic_unit_disk_generator(ComponentGenerator gen_component_)
      : gen_component(std::move(gen_component_)) {}

  constexpr vec3 operator()() {
    while (true) {
      auto p = vec3(std::invoke(gen_component), std::invoke(gen_component), 0);
      if (p.length_square() < 1)
        return p;
    }
  }
};

template <std::invocable ComponentGenerator>
basic_unit_disk_generator(ComponentGenerator)
    -> basic_unit_disk_generator<ComponentGenerator>;

struct random_unit_disk_generator
    : public basic_unit_disk_generator<random_double_generator> {
  random_unit_disk_generator()
      : basic_unit_disk_generator(random_double_generator(-1.0, 1.0)) {}
};
} // namespace mrl
