#include "aspect_ratio.hpp"
#include "camera.hpp"
#include "camera_orientation.hpp"
#include "hit_record.hpp"
#include "image/in_memory_image.hpp"
#include "image/ppm/ppm_utils.hpp"
#include "image_renderer.hpp"
#include "materials/concept.hpp"
#include "materials/lambertian.hpp"
#include "pixel_sampler/randomized_delta_sampler.hpp"
#include "scene.hpp"
#include "scene_objects/any_scene_object.hpp"
#include "scene_objects/debug/debug_object.hpp"
#include "scene_objects/debug/hooks/noop_hook.hpp"
#include "scene_objects/debug/hooks/ref_hook.hpp"
#include "scene_objects/debug/hooks/tagged_stream_hook.hpp"
#include "scene_objects/scene_object_range.hpp"
#include "scene_objects/shapes/sphere.hpp"
#include "std/random_double_generator.hpp"
#include "std/ranges.hpp"
#include "vector.hpp"
#include "viewport.hpp"
#include <iostream>

struct count_hook {
  int zero = 0;
  int neg = 0;
  int pos = 0;
  int nil = 0;

  void invoke_hit(mrl::ray_t const &ray,
                  std::optional<mrl::hit_record_t> const &h) {
    if (h) {
      auto const n = (*h).normal.val();
      auto const d = ray.direction.val();
      auto const res = mrl::dot(d, n);
      if (res == 0)
        ++zero;
      else if (res < 0)
        ++neg;
      else
        ++pos;
    } else {
      ++nil;
    }
  }
};

void debug() {
  mrl::aspect_ratio_t ratio{16, 9};
  auto scene = mrl::make_scene(ratio, 600);
  auto viewport = mrl::make_viewport(scene, 2.0);
  mrl::camera_t camera{
      .viewport = viewport,
      .focal_length = 1.0,
  };

  mrl::camera_orientation_t camera_orientation{
      .position = {0, 0, 0},
      .direction = mrl::vec3{0, 0, -1},
  };

  auto material = mrl::lambertian_t{mrl::color_t{0.5, 0.5, 0.5}};

  auto small_sphere = mrl::sphere_obj_t{0.5, {0, 0, -1}, material};
  auto big_sphere = mrl::sphere_obj_t{100, {0, -100.5, -1}, material};

  count_hook hook{};

  mrl::any_scene_object small =
      mrl::debug_obj_t{small_sphere, mrl::noop_hook{}};
  mrl::any_scene_object big = mrl::debug_obj_t{big_sphere, mrl::ref_hook{hook}};

  std::vector<mrl::any_scene_object> world{small, big};

  mrl::in_memory_image img{scene.width, scene.height};
  mrl::img_renderer_t renderer(camera, camera_orientation, 50,
                               mrl::randomized_delta_sampler{5});
  renderer.render(world, img);

  std::cerr << "nil : " << hook.nil << '\n'
            << "neg : " << hook.neg << '\n'
            << "zero : " << hook.zero << '\n'
            << "pos : " << hook.pos << '\n';
  mrl::write_ppm_img(std::cout, img);
}

void real() {
  mrl::aspect_ratio_t ratio{16, 9};
  auto scene = mrl::make_scene(ratio, 600);
  auto viewport = mrl::make_viewport(scene, 2.0);
  mrl::camera_t camera{
      .viewport = viewport,
      .focal_length = 1.0,
  };

  mrl::camera_orientation_t camera_orientation{
      .position = {0, 0, 0},
      .direction = mrl::vec3{0, 0, -1},
  };

  auto ground_material = mrl::lambertian_t{mrl::color_t{0.3, 0.3, 0.3}};
  auto ball_material = mrl::lambertian_t{mrl::color_t{0.3, 0.3, 0.3}};
  auto small_sphere = mrl::sphere_obj_t{0.5, {0, 0, -1}, ball_material};
  auto big_sphere = mrl::sphere_obj_t{100, {0, -100.5, -1}, ground_material};

  std::vector world{small_sphere, big_sphere};

  mrl::in_memory_image img{scene.width, scene.height};
  mrl::img_renderer_t renderer(camera, camera_orientation, 50,
                               mrl::randomized_delta_sampler{100});
  renderer.render(world, img);
  mrl::write_ppm_img(std::cout, img);
}

int main() { real(); }
