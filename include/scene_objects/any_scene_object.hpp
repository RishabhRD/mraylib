#pragma once

#include "hit_record.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include <memory>
#include <optional>

namespace mrl {
class any_scene_object {
public:
  template <Hittable T>
  any_scene_object(T x) : self_(std::make_shared(model_t<T>(std::move(x)))) {}

  friend std::optional<hit_record_t> hit(any_scene_object const &obj,
                                         ray_t const &ray) {
    return obj.self_->hit(ray);
  }

private:
  struct concept_t {
    virtual ~concept_t() = default;
    virtual std::optional<hit_record_t> hit(ray_t const &ray) const = 0;
  };

  template <Hittable T> struct model_t final : concept_t {
    T hittable;
    model_t(T h_arg) : hittable(std::move(h_arg)){};

    std::optional<hit_record_t> hit(ray_t const &ray) const override {
      return hit(hittable, ray);
    }
  };

  std::shared_ptr<concept_t const> self_;
};

std::optional<hit_record_t> hit(any_scene_object const &obj, ray_t const &ray,
                                point3 const &point);
} // namespace mrl
