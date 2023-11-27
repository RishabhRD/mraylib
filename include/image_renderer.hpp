#pragma once

#include "camera.hpp"
#include "camera_orientation.hpp"
#include "color.hpp"
#include "direction.hpp"
#include "image/concepts.hpp"
#include "pixel_sampler/concepts.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include "scene_objects/shapes/sphere.hpp"
#include "vector.hpp"
#include "viewport_utils.hpp"

namespace mrl {

template <SceneObject Object>
constexpr color_t ray_color(ray_t const &ray, Object const &world, int depth) {
  if (depth <= 0)
    return {0, 0, 0};
  auto hit_record = hit(world, ray);
  if (hit_record) {
    auto scattering = hit_record->scattering;
    if (!scattering.has_value())
      return {0, 0, 0};
    auto scattered_ray = scattering->scattered_ray;
    auto attenuation = scattering->attenuated_color;
    return attenuation * ray_color(scattered_ray, world, depth - 1);
  }
  auto dir = ray.direction.val();
  auto a = (dir.y + 1.0) / 2;
  return (1.0 - a) * color_t{1.0, 1.0, 1.0} + a * color_t{0.5, 0.7, 1.0};
}

// Precondition:
//   - std::ranges::distance(rng) >= 1
template <SceneObject Object, std::ranges::input_range VectorRange>
  requires RangeValueType<VectorRange, vec3>
constexpr color_t sampled_ray_color(vec3 const &camera_center,
                                    VectorRange const &pixel_points,
                                    Object const &world, int depth) {
  double num_ele = 0.0;
  color_t color{0, 0, 0};
  for (vec3 const &pixel_center : pixel_points) {
    ray_t r{
        .origin = camera_center,
        .direction = pixel_center - camera_center,
    };
    color += ray_color(r, world, depth);
    ++num_ele;
  }
  return color / num_ele;
}

template <SceneObject Object, RandomAccessImage Image, PixelSampler Sampler>
constexpr void render_image(Object const &world, Image &img,
                            camera_t const &camera,
                            camera_orientation_t const &orientation,
                            Sampler &sampler, int rendering_depth) {

  auto [u_dir, v_dir] = viewport_direction(orientation);
  auto viewport_u = u_dir * camera.viewport.width;
  auto viewport_v = v_dir * camera.viewport.height;

  auto pixel_delta_u = viewport_u / img.width();
  auto pixel_delta_v = viewport_v / img.height();

  auto viewport_top_left = viewport_topleft(viewport_u, viewport_v, orientation,
                                            camera.focal_length);

  auto pixel00_loc = viewport_top_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  for (int j = 0; j < img.height(); ++j) {
    for (int i = 0; i < img.width(); ++i) {
      auto pixel_center = pixel00_loc + i * pixel_delta_u + j * pixel_delta_v;
      auto sampling_points = sampler.sample({
          .point = pixel_center,
          .pixel_delta_u = pixel_delta_u,
          .pixel_delta_v = pixel_delta_v,
      });
      img.at(j, i) =
          sampled_ray_color(orientation.position, std::move(sampling_points),
                            world, rendering_depth);
    }
  }
}

template <PixelSampler Sampler> struct img_renderer_t {
  camera_t camera;
  camera_orientation_t camera_orientation;
  int rendering_depth;
  Sampler sampler;

  img_renderer_t(camera_t camera_, camera_orientation_t camera_orientation_,
                 int rendering_depth_, Sampler sampler_)
      : camera{std::move(camera_)},
        camera_orientation{std::move(camera_orientation_)},
        rendering_depth(rendering_depth_), sampler{std::move(sampler_)} {}

  template <SceneObject Object, RandomAccessImage Image>
  constexpr void render(Object const &world, Image &img) {
    render_image(world, img, camera, camera_orientation, sampler,
                 rendering_depth);
  }
};

template <PixelSampler Sampler>
img_renderer_t(camera_t, camera_orientation_t, Sampler)
    -> img_renderer_t<Sampler>;
} // namespace mrl
