#pragma once

namespace mrl {
template <typename T> struct dimension {
  using measure_type = T;
  T width;
  T height;
};
} // namespace mrl
