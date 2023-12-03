#pragma once

#include "std/random_double_generator.hpp"
#include <exec/inline_scheduler.hpp>
namespace mrl {
using inline_scheduler = exec::inline_scheduler;

} // namespace mrl

namespace stdexec {
namespace __inln {
inline auto random_generator(mrl::inline_scheduler) {
  return mrl::random_double_generator{};
}
} // namespace __inln
} // namespace stdexec
