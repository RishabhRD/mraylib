#pragma once

#include "generator/concepts.hpp"
#include "stdexec/execution.hpp"

namespace mrl {
template <typename scheduler_t>
concept Scheduler = stdexec::scheduler<scheduler_t> &&
                    requires(scheduler_t scheduler, unsigned long random_seed) {
                      {
                        random_generator(scheduler, random_seed)
                      } -> DoubleGenerator;
                    };
}
