#pragma once

#include "stdexec/execution.hpp"

namespace mrl {
template <typename scheduler_t>
concept Scheduler = stdexec::scheduler<scheduler_t>;
}
