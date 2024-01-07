#include "angle.hpp"
#include "aspect_ratio.hpp"
#include "camera/camera.hpp"
#include "camera/camera_orientation.hpp"
#include "color.hpp"
#include "direction.hpp"
#include "image/concepts.hpp"
#include "image/in_memory_image.hpp"
#include "image/ppm/ppm_utils.hpp"
#include "image/solid_color_image.hpp"
#include "image/stb_image.hpp"
#include "image_renderer.hpp"
#include "materials/concept.hpp"
#include "materials/dielectric.hpp"
#include "materials/lambertian.hpp"
#include "materials/metal.hpp"
#include "pixel_sampler/delta_sampler.hpp"
#include "point.hpp"
#include "scene_objects/any_scene_object.hpp"
#include "scene_objects/bvh.hpp"
#include "scene_objects/concepts.hpp"
#include "scene_objects/scene_object_range.hpp"
#include "scene_objects/shapes/sphere.hpp"
#include "schedulers/concepts.hpp"
#include "schedulers/inline_scheduler.hpp"
#include "schedulers/static_thread_pool_scheduler.hpp"
#include "schedulers/thread_pool.hpp"
#include "schedulers/type_traits.hpp"
#include "std/ranges.hpp"
#include "textures/checker_texture.hpp"
#include "textures/image_texture.hpp"
#include "textures/perlin_texture.hpp"
#include "textures/solid_color.hpp"
#include "vector.hpp"
#include <chrono>
#include <cstdlib>
#include <exec/async_scope.hpp>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexec/execution.hpp>

#define TH_POOL                                                                \
  auto const num_threads = std::thread::hardware_concurrency();                \
  auto thread_pool = mrl::thread_pool{num_threads};                            \
  auto sch = thread_pool.get_scheduler();

#define INLINE auto sch = mrl::inline_scheduler{};

void random_spheres() {
  auto const num_threads = std::thread::hardware_concurrency();
  auto thread_pool = mrl::thread_pool{num_threads};
  auto sch = thread_pool.get_scheduler();
  using any_object = mrl::any_object_t<decltype(sch)>;

  auto cur_time = static_cast<unsigned long>(
      std::chrono::system_clock::now().time_since_epoch().count());
  auto rand = random_generator(sch, cur_time);

  mrl::aspect_ratio_t ratio{16, 9};
  auto img_width = 1000;
  auto img_height = mrl::image_height(ratio, img_width);
  mrl::in_memory_image img{img_width, img_height};
  mrl::camera_t camera{
      .focus_distance = 10.0,
      .vertical_fov = mrl::degrees(20),
      .defocus_angle = mrl::degrees(0.0),
  };

  mrl::camera_orientation_t camera_orientation{
      .look_from = {13, 2, 3},
      .look_at = mrl::point3(0, 0, 0),
      .up_dir = mrl::direction_t{0, 1, 0},
  };

  std::vector<any_object> world;
  mrl::dielectric mat1(1.5);
  mrl::lambertian_t mat2(mrl::color_t{0.4, 0.2, 0.1});
  mrl::metal_t mat3(mrl::color_t{0.7, 0.6, 0.5});

  auto checker = mrl::lambertian_t{mrl::checker_texture{
      0.32, mrl::solid_color_texture{mrl::from_rgb(38, 39, 41)},
      mrl::solid_color_texture{mrl::color_t{.9, .9, .9}}}};

  world.push_back(mrl::sphere_obj_t{1.0, mrl::point3{0, 1, 0}, mat1});
  world.push_back(mrl::sphere_obj_t{1.0, mrl::point3{-4, 1, 0}, mat2});
  world.push_back(mrl::sphere_obj_t{1.0, mrl::point3{4, 1, 0}, mat3});
  world.push_back(mrl::sphere_obj_t{1000.0, mrl::point3{0, -1000, 0}, checker});

  for (int a = -11; a < 11; ++a) {
    for (int b = -11; b < 11; ++b) {
      auto choose_mat = rand(0.0, 1.0);
      auto center =
          mrl::point3{a + 0.9 * rand(0.0, 1.0), 0.2, b * 0.9 * rand(0.0, 1.0)};
      if (choose_mat < 0.8) {
        auto color =
            mrl::color_t{rand(0.0, 1.0), rand(0.0, 1.0), rand(0.0, 1.0)} *
            mrl::color_t{rand(0.0, 1.0), rand(0.0, 1.0), rand(0.0, 1.0)};
        world.push_back(
            mrl::sphere_obj_t{0.2, center, mrl::lambertian_t{color}});
      } else if (choose_mat < 0.95) {
        auto color =
            mrl::color_t{rand(0.0, 1.0), rand(0.0, 1.0), rand(0.0, 1.0)};
        auto fuzz = rand(0.5, 1.0);
        world.push_back(
            mrl::sphere_obj_t{0.2, center, mrl::fuzzy_metal_t{color, fuzz}});

      } else {
        world.push_back(mrl::sphere_obj_t{0.2, center, mrl::dielectric{1.5}});
      }
    }
  }

  mrl::bvh_t<any_object> bvh{std::move(world)};
  auto path = std::getenv("HOME") + std::string{"/x.ppm"};
  std::ofstream os(path, std::ios::out);
  mrl::img_renderer_t renderer(camera, camera_orientation, sch, cur_time);
  stdexec::sync_wait(renderer.render(bvh, img));
  mrl::write_ppm_img(os, img);
}

void img() {
  auto space_img = mrl::stb_image{"data/space.jpg"};
  auto const img_width = width(space_img);
  auto const img_height = height(space_img);
  std::cerr << img_width << ' ' << img_height << std::endl;
  mrl::in_memory_image img{img_width, img_height};
  for (int y = 0; y < img_height; ++y) {
    for (int x = 0; x < img_width; ++x) {
      set_pixel_at(img, x, y, pixel_at(space_img, x, y));
    }
  }
  auto path = std::getenv("HOME") + std::string{"/x.ppm"};
  std::ofstream os(path, std::ios::out);
  mrl::write_ppm_img_without_gamma(os, img);
}

void earth() {
  TH_POOL

  auto cur_time = static_cast<unsigned long>(
      std::chrono::system_clock::now().time_since_epoch().count());

  auto img_width = 1000;
  mrl::aspect_ratio_t ratio{16, 9};
  auto img_height = mrl::image_height(ratio, img_width);
  mrl::camera_t camera{
      .focus_distance = 10.0,
      .vertical_fov = mrl::degrees(20),
      .defocus_angle = mrl::degrees(0),
  };
  mrl::camera_orientation_t camera_orientation{
      .look_from = mrl::point3{0, 0, 0},
      .look_at = mrl::point3{0, 0, 15},
      .up_dir = mrl::direction_t{0, 1, 0},
  };

  auto earth_img = mrl::stb_image{"data/earthmap.jpg"};
  auto earth_texture = mrl::image_texture{std::move(earth_img)};
  auto earth_surface = mrl::lambertian_t{std::move(earth_texture)};
  auto globe =
      mrl::sphere_obj_t{2, mrl::point3{0, 0, 15}, std::move(earth_surface)};

  auto space_img = mrl::stb_image{"data/space.jpg"};
  auto space_texture = mrl::image_texture{std::move(space_img)};
  auto space_surface = mrl::lambertian_t{std::move(space_texture)};
  auto space =
      mrl::sphere_obj_t{10, mrl::point3{0, 0, 25}, std::move(space_surface)};

  std::vector<decltype(space)> world;
  world.push_back(std::move(globe));
  world.push_back(std::move(space));

  mrl::in_memory_image img{img_width, img_height};
  auto path = std::getenv("HOME") + std::string{"/x.ppm"};
  std::ofstream os(path, std::ios::out);
  mrl::img_renderer_t renderer(camera, camera_orientation, sch, cur_time);
  stdexec::sync_wait(renderer.render(world, img));
  mrl::write_ppm_img_without_gamma(os, img);
}

void perlin_spheres() {
  TH_POOL

  auto cur_time = static_cast<unsigned long>(
      std::chrono::system_clock::now().time_since_epoch().count());

  auto img_width = 1000;
  mrl::aspect_ratio_t ratio{16, 9};
  auto img_height = mrl::image_height(ratio, img_width);
  mrl::camera_t camera{
      .focus_distance = 10.0,
      .vertical_fov = mrl::degrees(20),
      .defocus_angle = mrl::degrees(0),
  };
  mrl::camera_orientation_t camera_orientation{
      .look_from = mrl::point3{13, 2, 3},
      .look_at = mrl::point3{0, 0, 0},
      .up_dir = mrl::direction_t{0, 1, 0},
  };

  mrl::perlin_texture texture{mrl::solid_color_texture{1, 1, 1},
                              mrl::perlin_noise{cur_time}, 4};
  mrl::lambertian_t material{texture};
  mrl::sphere_obj_t big_sphere{1000, mrl::point3{0, -1000, 0}, material};
  mrl::sphere_obj_t small_sphere{2, mrl::point3{0, 2, 0}, material};

  std::vector world{big_sphere, small_sphere};

  mrl::in_memory_image img{img_width, img_height};
  auto path = std::getenv("HOME") + std::string{"/x.ppm"};
  std::ofstream os(path, std::ios::out);
  mrl::img_renderer_t renderer(camera, camera_orientation, sch, cur_time);
  stdexec::sync_wait(renderer.render(world, img));
  mrl::write_ppm_img(os, img);
}

int main() { perlin_spheres(); }
