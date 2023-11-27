#pragma once

#include <ostream>

namespace mrl {

template <typename T>
concept Streamable = requires(T value, std::ostream &os) {
  { os << value } -> std::convertible_to<std::ostream &>;
};

}
