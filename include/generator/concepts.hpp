#pragma once

#include <concepts>
namespace mrl {

// Postcondition: generates a value in range [min, max)
template <typename generator_t, typename ValueType>
concept Generator =
    std::totally_ordered<ValueType> &&
    requires(generator_t &gen, ValueType const &min, ValueType const &max) {
      { gen(min, max) } -> std::same_as<ValueType>;
    };

template <typename random_generator_t>
concept DoubleGenerator = Generator<random_generator_t, double>;
} // namespace mrl
