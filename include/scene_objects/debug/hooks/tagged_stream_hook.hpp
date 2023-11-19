#pragma once

#include "../../../concepts.hpp"
#include "hit_record.hpp"
#include "optional_utils.hpp"
#include "ray.hpp"
#include <optional>
#include <ostream>

namespace mrl {
template <Streamable T> class tagged_stream_hook {
private:
  std::ostream *stream_;
  T tag_;

public:
  tagged_stream_hook(std::ostream &stream, T tag)
      : stream_(&stream), tag_(std::move(tag)) {}

  void invoke_hit(ray_t const &r, std::optional<hit_record_t> const &h) const {
    (*stream_) << "-----Hit-----nTag: " << tag_ << "\nRay: " << r
               << "\nHit Record: " << h << "\n\n";
  }
};

template <Streamable T>
tagged_stream_hook(std::ostream &stream, T tag) -> tagged_stream_hook<T>;
} // namespace mrl
