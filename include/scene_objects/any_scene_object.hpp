#pragma once

#include "hit_record.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include <memory>
#include <optional>

namespace mrl {
class any_scene_object {
public:
  // TODO: Using SceneObject here is creating problem to use it with
  // scene_object_list with any algorithm that depends on SceneObject.
  //
  // template <SceneObject T>
  template <typename T>
  any_scene_object(T x) : self_(std::make_shared<model_t<T>>(std::move(x))) {}

  friend std::optional<hit_record_t>
  hit(any_scene_object const &obj, ray_t const &ray, interval_t const &t_rng) {
    return obj.self_->hit_mem(ray, t_rng);
  }

private:
  struct concept_t {
    virtual ~concept_t() = default;
    virtual std::optional<hit_record_t> hit_mem(ray_t const &,
                                                interval_t const &) const = 0;
  };

  template <SceneObject T> struct model_t final : concept_t {
    T hittable;
    model_t(T h_arg) : hittable(std::move(h_arg)){};

    std::optional<hit_record_t>
    hit_mem(ray_t const &ray, interval_t const &t_rng) const override {
      return hit(hittable, ray, t_rng);
    }
  };

  std::shared_ptr<concept_t const> self_;
};

std::optional<hit_record_t> hit(any_scene_object const &, ray_t const &,
                                interval_t const &);
} // namespace mrl
