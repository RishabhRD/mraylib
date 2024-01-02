#pragma once

#include "camera/camera.hpp"
#include "camera/camera_orientation.hpp"
#include "camera/concepts.hpp"
#include "color.hpp"
#include "direction.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "generator/unit_disk_generator.hpp"
#include "image/concepts.hpp"
#include "image/in_memory_image.hpp"
#include "interval.hpp"
#include "pixel_sampler/concepts.hpp"
#include "pixel_sampler/delta_sampler.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include "scene_objects/shapes/sphere.hpp"
#include "schedulers/concepts.hpp"
#include "schedulers/type_traits.hpp"
#include "std/ranges.hpp"
#include "vector.hpp"
#include <cmath>
#include <functional>
#include <limits>
#include <ranges>
#include <stdexec/execution.hpp>
#include <type_traits>

namespace mrl {

struct rendering_context_t {
  vec3 pixel_delta_u;
  vec3 pixel_delta_v;
  point3 pixel00_loc;
  vec3 defocus_disk_u;
  vec3 defocus_disk_v;
  int rendering_depth;
  vec3 camera_position;
};

constexpr std::pair<vec3, vec3> viewport_direction(direction_t camera_dir,
                                                   direction_t up_dir) {
  auto right = normalize(cross(camera_dir.val(), up_dir.val()));
  return {right, -up_dir.val()};
}

constexpr vec3 viewport_topleft(vec3 right, vec3 down, point3 camera_pos,
                                direction_t camera_dir, double f) {
  return camera_pos + camera_dir.val() * f - right / 2 - down / 2;
}

template <DoubleGenerator Generator, SceneObject<Generator> Object>
constexpr color_t ray_color(ray_t const &ray, Object const &world, int depth,
                            generator_view<Generator> rand) {
  if (depth <= 0)
    return {0, 0, 0};
  constexpr static double closeness_limit = 0.001;
  auto hit_record =
      hit(world, ray,
          interval_t{closeness_limit, std::numeric_limits<double>::infinity()},
          rand);
  if (hit_record) {
    auto scattering = hit_record->scattering;
    if (!scattering.has_value())
      return {0, 0, 0};
    auto scattered_ray = scattering->scattered_ray;
    auto attenuation = scattering->attenuated_color;
    return attenuation * ray_color(scattered_ray, world, depth - 1, rand);
  }
  auto dir = ray.direction.val();
  auto a = (dir.y + 1.0) / 2;
  return (1.0 - a) * color_t{1.0, 1.0, 1.0} + a * color_t{0.5, 0.7, 1.0};
}

// Precondition:
//   - std::ranges::distance(rng) >= 1
//   - RayOriginGenerator generates center in defocus disk
template <DoubleGenerator Generator, SceneObject<Generator> Object,
          std::ranges::input_range VectorRange,
          std::invocable RayOriginGenerator>
  requires RangeValueType<VectorRange, vec3> &&
           std::same_as<std::invoke_result_t<RayOriginGenerator>, vec3>
constexpr color_t sampled_ray_color(RayOriginGenerator &gen_center,
                                    VectorRange const &pixel_points,
                                    Object const &world, int depth,
                                    generator_view<Generator> rand) {
  double num_ele = 0.0;
  color_t color{0, 0, 0};
  for (vec3 const &pixel_center : pixel_points) {
    auto ray_origin = std::invoke(gen_center);
    ray_t r{
        .origin = ray_origin,
        .direction = pixel_center - ray_origin,
    };
    color += ray_color(r, world, depth, rand);
    ++num_ele;
  }
  return color / num_ele;
}

template <Camera camera_t, OutputRandomAccessImage Image>
constexpr auto build_rendering_context(Image &img, camera_t const &camera,
                                       camera_orientation_t const &orientation,
                                       int rendering_depth) {
  auto focus_dist = focus_distance(camera);
  auto h = std::tan(vertical_fov(camera).radians / 2);
  auto viewport_height = 2 * h * focus_dist;
  auto viewport_width =
      viewport_height * (static_cast<double>(width(img)) / height(img));
  auto camera_dir = direction_t{orientation.look_at - orientation.look_from};
  auto [u_dir, v_dir] = viewport_direction(camera_dir, orientation.up_dir);
  auto viewport_u = u_dir * viewport_width;
  auto viewport_v = v_dir * viewport_height;

  auto pixel_delta_u = viewport_u / width(img);
  auto pixel_delta_v = viewport_v / height(img);

  auto viewport_top_left = viewport_topleft(
      viewport_u, viewport_v, orientation.look_from, camera_dir, focus_dist);

  auto pixel00_loc = viewport_top_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  auto defocus_radius =
      focus_dist * std::tan(defocus_angle(camera).radians / 2);
  auto defocus_disk_u = u_dir * defocus_radius;
  auto defocus_disk_v = (-v_dir) * defocus_radius;
  return rendering_context_t{
      .pixel_delta_u = pixel_delta_u,
      .pixel_delta_v = pixel_delta_v,
      .pixel00_loc = pixel00_loc,
      .defocus_disk_u = defocus_disk_u,
      .defocus_disk_v = defocus_disk_v,
      .rendering_depth = rendering_depth,
      .camera_position = orientation.look_from,
  };
}

template <DoubleGenerator random_t, SceneObject<random_t> Object,
          PixelSampler<random_t> Sampler>
constexpr auto generate_pixel(int row, int col, Object const &world,
                              rendering_context_t ctx, Sampler sampler,
                              generator_view<random_t> rand) {
  auto ray_origin_generator = [ctx, rand] {
    auto p = unit_disk_generator{}(rand);
    return ctx.camera_position + ctx.defocus_disk_u * p.x +
           ctx.defocus_disk_v * p.y;
  };
  auto pixel_center =
      ctx.pixel00_loc + col * ctx.pixel_delta_u + row * ctx.pixel_delta_v;
  auto sampling_points = sampler(
      {
          .point = pixel_center,
          .pixel_delta_u = ctx.pixel_delta_u,
          .pixel_delta_v = ctx.pixel_delta_v,
      },
      rand);
  return sampled_ray_color(ray_origin_generator, std::move(sampling_points),
                           world, ctx.rendering_depth, rand);
}

template <Camera camera_t, OutputRandomAccessImage Image, Scheduler scheduler_t,
          DoubleGenerator random_t, SceneObject<random_t> Object,
          PixelSampler<random_t> Sampler>
constexpr auto
render_image(Object const &world, Image &img, camera_t const &camera,
             camera_orientation_t const &orientation, Sampler sampler,
             int rendering_depth, scheduler_t scheduler,
             generator_view<random_t> rand) {
  auto rendering_ctx =
      build_rendering_context(img, camera, orientation, rendering_depth);

  auto set_pixel_at_coord = [&img, &world, rendering_ctx, sampler,
                             rand](auto coord) {
    auto row = coord / width(img);
    auto col = coord % width(img);
    auto color = generate_pixel(row, col, world, rendering_ctx, sampler, rand);
    set_pixel_at(img, row, col, color);
  };

  return stdexec::schedule(scheduler) |
         stdexec::bulk(height(img) * width(img), set_pixel_at_coord);
}

template <Camera camera_t, Scheduler scheduler_t,
          typename Sampler = delta_sampler>
struct img_renderer_t {
  using random_generator_t = scheduler_random_generator_t<scheduler_t>;
  static_assert(PixelSampler<Sampler, random_generator_t>);
  camera_t camera;
  camera_orientation_t camera_orientation;
  scheduler_t scheduler;
  random_generator_t gen;
  int rendering_depth;
  Sampler sampler;

  img_renderer_t(camera_t camera_, camera_orientation_t camera_orientation_,
                 scheduler_t scheduler_, random_generator_t rand_,
                 int rendering_depth_ = 50,
                 Sampler sampler_ = delta_sampler(100))
      : camera{std::move(camera_)},
        camera_orientation{std::move(camera_orientation_)},
        scheduler(std::move(scheduler_)), gen(std::move(rand_)),
        rendering_depth(rendering_depth_), sampler{std::move(sampler_)} {}

  img_renderer_t(camera_t camera_, camera_orientation_t camera_orientation_,
                 scheduler_t scheduler_, unsigned long random_seed = 0,
                 int rendering_depth_ = 100,
                 Sampler sampler_ = delta_sampler(50))
      : img_renderer_t(std::move(camera_), std::move(camera_orientation_),
                       scheduler_, random_generator(scheduler_, random_seed),
                       rendering_depth_, std::move(sampler_)) {}

  template <SceneObject<random_generator_t> Object,
            OutputRandomAccessImage Image>
  constexpr auto render(Object const &world, Image &img) {
    return render_image(world, img, camera, camera_orientation, sampler,
                        rendering_depth, scheduler, generator_view{gen});
  }
};

template <Camera camera_t, Scheduler scheduler_t, typename Sampler>
img_renderer_t(camera_t, camera_orientation_t, scheduler_t,
               typename img_renderer_t<camera_t, scheduler_t,
                                       Sampler>::random_generator_t &,
               int, Sampler) -> img_renderer_t<camera_t, scheduler_t, Sampler>;

template <Camera camera_t, Scheduler scheduler_t, typename Sampler>
img_renderer_t(camera_t, camera_orientation_t, scheduler_t, int, Sampler)
    -> img_renderer_t<camera_t, scheduler_t, Sampler>;
} // namespace mrl
