#pragma once

#include "direction.hpp"
#include "std/random_double_generator.hpp"
#include "vector.hpp"
#include <functional>
namespace mrl {
template <std::invocable ComponentGenerator> struct basic_direction_generator {
  ComponentGenerator gen_component;

  basic_direction_generator(ComponentGenerator gen_component_)
      : gen_component(std::move(gen_component_)) {}

  constexpr direction_t operator()() {
    return direction_t{vec3{
        std::invoke(gen_component),
        std::invoke(gen_component),
        std::invoke(gen_component),
    }};
  }
};

template <std::invocable ComponentGenerator>
basic_direction_generator(ComponentGenerator)
    -> basic_direction_generator<ComponentGenerator>;

struct random_direction_generator
    : public basic_direction_generator<random_double_generator> {
  random_direction_generator()
      : basic_direction_generator(random_double_generator(-1.0, 1.0)) {}
};
} // namespace mrl
