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
#include "materials/diffuse_light.hpp"
#include "materials/lambertian.hpp"
#include "materials/metal.hpp"
#include "pixel_sampler/delta_sampler.hpp"
#include "point.hpp"
#include "scene_objects/any_scene_object.hpp"
#include "scene_objects/bvh.hpp"
#include "scene_objects/concepts.hpp"
#include "scene_objects/scene_object_range.hpp"
#include "scene_objects/shapes/quad.hpp"
#include "scene_objects/shapes/shape_object.hpp"
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

using namespace mrl;

#define TH_POOL                                                                \
  auto const num_threads = std::thread::hardware_concurrency();                \
  auto th_pool = thread_pool{num_threads};                                     \
  auto sch = th_pool.get_scheduler();

#define INLINE auto sch = inline_scheduler{};

#define ANY using any_object = any_object_t<decltype(sch)>;

#define BACKGROUND auto background = color_t{0.7, 0.8, 1.0};

#define RENDER                                                                 \
  in_memory_image img{img_width, img_height};                                  \
  bvh_t<any_object> bvh{std::move(world)};                                     \
  auto path = std::getenv("HOME") + std::string{"/x.ppm"};                     \
  std::ofstream os(path, std::ios::out);                                       \
  img_renderer_t renderer(camera, camera_orientation, background, sch,         \
                          cur_time);                                           \
  stdexec::sync_wait(renderer.render(bvh, img));                               \
  write_ppm_img(os, img);

void random_spheres() {
  TH_POOL
  ANY;
  BACKGROUND
  auto cur_time = static_cast<unsigned long>(
      std::chrono::system_clock::now().time_since_epoch().count());
  auto rand = random_generator(sch, cur_time);

  aspect_ratio_t ratio{16, 9};
  auto img_width = 1000;
  auto img_height = image_height(ratio, img_width);
  camera_t camera{
      .focus_distance = 10.0,
      .vertical_fov = degrees(20),
      .defocus_angle = degrees(0.0),
  };

  camera_orientation_t camera_orientation{
      .look_from = {13, 2, 3},
      .look_at = point3(0, 0, 0),
      .up_dir = direction_t{0, 1, 0},
  };

  std::vector<any_object> world;
  dielectric mat1(1.5);
  lambertian_t mat2(color_t{0.4, 0.2, 0.1});
  metal_t mat3(color_t{0.7, 0.6, 0.5});

  auto checker = lambertian_t{
      checker_texture{0.32, solid_color_texture{from_rgb(38, 39, 41)},
                      solid_color_texture{color_t{.9, .9, .9}}}};

  world.push_back(shape_object{sphere{1.0, point3{0, 1, 0}}, mat1});
  world.push_back(shape_object{sphere{1.0, point3{-4, 1, 0}}, mat2});
  world.push_back(shape_object{sphere{1.0, point3{4, 1, 0}}, mat3});
  world.push_back(shape_object{sphere{1000.0, point3{0, -1000, 0}}, checker});

  for (int a = -11; a < 11; ++a) {
    for (int b = -11; b < 11; ++b) {
      auto choose_mat = rand(0.0, 1.0);
      auto center =
          point3{a + 0.9 * rand(0.0, 1.0), 0.2, b * 0.9 * rand(0.0, 1.0)};
      if (choose_mat < 0.8) {
        auto color = color_t{rand(0.0, 1.0), rand(0.0, 1.0), rand(0.0, 1.0)} *
                     color_t{rand(0.0, 1.0), rand(0.0, 1.0), rand(0.0, 1.0)};
        world.push_back(shape_object{sphere{0.2, center}, lambertian_t{color}});
      } else if (choose_mat < 0.95) {
        auto color = color_t{rand(0.0, 1.0), rand(0.0, 1.0), rand(0.0, 1.0)};
        auto fuzz = rand(0.5, 1.0);
        world.push_back(
            shape_object{sphere{0.2, center}, fuzzy_metal_t{color, fuzz}});

      } else {
        world.push_back(shape_object{sphere{0.2, center}, dielectric{1.5}});
      }
    }
  }

  RENDER
}

void img() {
  auto space_img = stb_image{"data/space.jpg"};
  auto const img_width = width(space_img);
  auto const img_height = height(space_img);
  std::cerr << img_width << ' ' << img_height << std::endl;
  in_memory_image img{img_width, img_height};
  for (int y = 0; y < img_height; ++y) {
    for (int x = 0; x < img_width; ++x) {
      set_pixel_at(img, x, y, pixel_at(space_img, x, y));
    }
  }
  auto path = std::getenv("HOME") + std::string{"/x.ppm"};
  std::ofstream os(path, std::ios::out);
  write_ppm_img_without_gamma(os, img);
}

void earth() {
  TH_POOL
  ANY;
  BACKGROUND

  auto cur_time = static_cast<unsigned long>(
      std::chrono::system_clock::now().time_since_epoch().count());
  auto img_width = 1000;
  aspect_ratio_t ratio{16, 9};
  auto img_height = image_height(ratio, img_width);
  camera_t camera{
      .focus_distance = 10.0,
      .vertical_fov = degrees(20),
      .defocus_angle = degrees(0),
  };
  camera_orientation_t camera_orientation{
      .look_from = point3{0, 0, 0},
      .look_at = point3{0, 0, 15},
      .up_dir = direction_t{0, 1, 0},
  };

  auto earth_img = stb_image{"data/earthmap.jpg"};
  auto earth_texture = image_texture{std::move(earth_img)};
  auto earth_surface = lambertian_t{std::move(earth_texture)};
  auto globe =
      shape_object{sphere{2, point3{0, 0, 15}}, std::move(earth_surface)};

  auto space_img = stb_image{"data/space.jpg"};
  auto space_texture = image_texture{std::move(space_img)};
  auto space_surface = lambertian_t{std::move(space_texture)};
  auto space =
      shape_object{sphere{10, point3{0, 0, 25}}, std::move(space_surface)};

  std::vector<decltype(space)> world;
  world.push_back(std::move(globe));
  world.push_back(std::move(space));

  RENDER
}

void perlin_spheres() {
  TH_POOL
  ANY;
  BACKGROUND

  auto cur_time = static_cast<unsigned long>(
      std::chrono::system_clock::now().time_since_epoch().count());

  auto img_width = 1000;
  aspect_ratio_t ratio{16, 9};
  auto img_height = image_height(ratio, img_width);
  camera_t camera{
      .focus_distance = 10.0,
      .vertical_fov = degrees(20),
      .defocus_angle = degrees(0),
  };
  camera_orientation_t camera_orientation{
      .look_from = point3{13, 2, 3},
      .look_at = point3{0, 0, 0},
      .up_dir = direction_t{0, 1, 0},
  };

  auto earth_img = stb_image{"data/human.jpeg"};
  auto earth_texture = image_texture{std::move(earth_img)};
  perlin_texture texture{solid_color_texture{0.78, 0.4, 0.1},
                         perlin_noise{cur_time}, 4};
  perlin_texture small_texture{std::move(earth_texture), perlin_noise{cur_time},
                               4};
  lambertian_t material{texture};
  lambertian_t small_material{std::move(small_texture)};
  shape_object big_sphere{sphere{1000, point3{0, -1000, 0}}, material};
  shape_object small_sphere{sphere{2, point3{0, 2, 0}},
                            std::move(small_material)};

  std::vector<any_object> world{big_sphere, std::move(small_sphere)};

  RENDER
}

void quads() {
  TH_POOL
  ANY;
  BACKGROUND

  auto cur_time = static_cast<unsigned long>(
      std::chrono::system_clock::now().time_since_epoch().count());

  lambertian_t left_red{color_t(1.0, 0.2, 0.2)};
  lambertian_t back_green{color_t(0.2, 1.0, 0.2)};
  lambertian_t right_blue{color_t(0.2, 0.2, 1.0)};
  lambertian_t upper_orange{color_t(1.0, 0.5, 0.0)};
  lambertian_t lower_teal{color_t(0.2, 0.8, 0.8)};

  std::vector world{
      shape_object{quad{point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0)},
                   left_red},
      shape_object{quad{point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0)},
                   back_green},
      shape_object{quad{point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0)},
                   right_blue},
      shape_object{quad{point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4)},
                   upper_orange},
      shape_object{quad{point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4)},
                   lower_teal},
  };

  auto img_width = 1000;
  aspect_ratio_t ratio{1, 1};
  auto img_height = image_height(ratio, img_width);
  camera_t camera{
      .focus_distance = 10,
      .vertical_fov = degrees(80),
      .defocus_angle = degrees(0),
  };
  camera_orientation_t camera_orientation{
      .look_from = point3{0, 0, 9},
      .look_at = point3{0, 0, 0},
      .up_dir = direction_t{0, 1, 0},
  };

  RENDER
}

void simple_light() {
  TH_POOL
  ANY;
  auto background = color_t{0, 0, 0};

  auto cur_time = static_cast<unsigned long>(
      std::chrono::system_clock::now().time_since_epoch().count());

  auto img_width = 1000;
  aspect_ratio_t ratio{16, 9};
  auto img_height = image_height(ratio, img_width);
  camera_t camera{
      .focus_distance = 10.0,
      .vertical_fov = degrees(20),
      .defocus_angle = degrees(0),
  };
  camera_orientation_t camera_orientation{
      .look_from = point3{26, 3, 6},
      .look_at = point3{0, 2, 0},
      .up_dir = direction_t{0, 1, 0},
  };

  auto earth_img = stb_image{"data/human.jpeg"};
  auto earth_texture = image_texture{std::move(earth_img)};
  auto earth_surface = lambertian_t{std::move(earth_texture)};
  perlin_texture texture{solid_color_texture{0.78, 0.4, 0.1},
                         perlin_noise{cur_time}, 4};
  lambertian_t material{texture};
  diffuse_light light{color_t{1, 1, 1}, 20};
  shape_object big_sphere{sphere{1000, point3{0, -1000, 0}}, material};
  shape_object small_sphere{sphere{2, point3{0, 2, 0}},
                            std::move(earth_surface)};
  shape_object light_source{
      quad{point3{3, 1, -2}, vec3{2, 0, 0}, vec3{0, 3, 0}}, std::move(light)};

  std::vector<any_object> world{big_sphere, std::move(small_sphere),
                                std::move(light_source)};

  RENDER
}

int main() { simple_light(); }
