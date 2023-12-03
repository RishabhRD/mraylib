#pragma once

#include "generator/concepts.hpp"
#include "stdexec/execution.hpp"

namespace mrl {
template <typename scheduler_t>
concept Scheduler = stdexec::scheduler<scheduler_t>
    && requires(scheduler_t scheduler) {
      { random_generator(scheduler) } -> DoubleGenerator;
    };
}
