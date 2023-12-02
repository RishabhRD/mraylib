#pragma once

#include "generator/concepts.hpp"
namespace mrl {
template <typename InnerGenerator> struct generator_view {
  using generator_type = InnerGenerator;
  InnerGenerator *gen;

  explicit generator_view(InnerGenerator &gen_) : gen(&gen_) {}

  template <typename T>
    requires Generator<InnerGenerator, T>
  auto operator()(T const &min, T const &max) {
    return (*gen)(min, max);
  }
};

template <typename Generator>
generator_view(Generator &) -> generator_view<Generator>;
} // namespace mrl
