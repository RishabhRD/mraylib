#include "mraylib.hpp"
#include <fstream>
#include <vector>

// Output:
// https://github.com/RishabhRD/mraylib/assets/26287448/4b53c022-f152-4794-a327-012b9673ae85

using namespace mrl;
namespace rng = std::ranges;

auto box(point3 const &a, point3 const &b, auto material) {
  std::vector<shape_object<quad, decltype(material)>> res;
  auto min =
      point3(std::fmin(a.x, b.x), std::fmin(a.y, b.y), std::fmin(a.z, b.z));
  auto max =
      point3(std::fmax(a.x, b.x), std::fmax(a.y, b.y), std::fmax(a.z, b.z));
  auto dx = vec3(max.x - min.x, 0, 0);
  auto dy = vec3(0, max.y - min.y, 0);
  auto dz = vec3(0, 0, max.z - min.z);
  res.push_back(
      shape_object{quad{point3{min.x, min.y, max.z}, dx, dy}, material});
  res.push_back(
      shape_object{quad{point3{max.x, min.y, max.z}, -dz, dy}, material});
  res.push_back(
      shape_object{quad{point3{max.x, min.y, min.z}, -dx, dy}, material});
  res.push_back(
      shape_object{quad{point3{min.x, min.y, min.z}, dz, dy}, material});
  res.push_back(
      shape_object{quad{point3{min.x, max.y, max.z}, dx, -dz}, material});
  res.push_back(
      shape_object{quad{point3{min.x, min.y, min.z}, dx, dz}, material});
  return res;
}

auto move_by(vec3 offset) {
  return [offset](auto obj) {
    return translate_object{std::move(obj), offset};
  };
}

auto rotate_by(ray_t axis, angle_t angle) {
  return [axis, angle](auto obj) {
    return rotate_object{std::move(obj), axis, angle};
  };
}

int main() {
  // Configure Execution Context
  auto const num_threads = std::thread::hardware_concurrency();
  auto th_pool = thread_pool{num_threads};
  auto sch = th_pool.get_scheduler();

  // Configure camera (how we look into the world)
  camera_t camera{
      .focus_distance = 10.0,
      .vertical_fov = degrees(40),
      .defocus_angle = degrees(0),
  };
  camera_orientation_t camera_orientation{
      .look_from = point3{278, 278, -800},
      .look_at = point3{278, 278, 0},
      .up_dir = direction_t{0, 1, 0},
  };

  // Define the world
  auto red = lambertian_t{color_t{.65, .05, .05}};
  auto white = lambertian_t{color_t{.73, .73, .73}};
  auto green = lambertian_t{color_t{.12, .45, .15}};
  auto light = diffuse_light{color_t{15, 15, 15}};

  using any_object = any_object_t<decltype(sch)>;

  std::vector<any_object> world;
  world.push_back(shape_object{
      quad{point3{555, 0, 0}, vec3{0, 555, 0}, vec3{0, 0, 555}}, green});
  world.push_back(shape_object{
      quad{point3{0, 0, 0}, vec3{0, 555, 0}, vec3{0, 0, 555}}, red});
  world.push_back(shape_object{
      quad{point3{343, 554, 332}, vec3{-130, 0, 0}, vec3{0, 0, -105}}, light});
  world.push_back(shape_object{
      quad{point3{0, 0, 0}, vec3{555, 0, 0}, vec3{0, 0, 555}}, white});
  world.push_back(shape_object{
      quad{point3{555, 555, 555}, vec3{-555, 0, 0}, vec3{0, 0, -555}}, white});
  world.push_back(shape_object{
      quad{point3{0, 0, 555}, vec3{555, 0, 0}, vec3{0, 555, 0}}, white});
  auto box1 = box(point3(265, 0, 295), point3(430, 330, 460), white);
  auto box2 = box(point3(130, 0, 65), point3(295, 165, 230), white);
  auto axis_of_rotation = ray_t{
      (point3(130, 0, 65) + point3(295, 0, 230)) / 2.0, direction_t{0, 1, 0}};
  auto angle_of_rotation = degrees(-22);
  rng::transform(box1, std::back_inserter(world),
                 rotate_by(axis_of_rotation, angle_of_rotation));
  rng::transform(box2, std::back_inserter(world), move_by(vec3{0, 100, 0}));

  bvh_t<any_object> bvh{std::move(world)}; // Acceleration Structure
  auto background = color_t{0, 0, 0};

  // Define the image
  auto img_width = 600;
  aspect_ratio_t ratio{1, 1};
  auto img_height = image_height(ratio, img_width);
  in_memory_image img{img_width, img_height};
  auto path = std::getenv("HOME") + std::string{"/cornell_box.ppm"};
  std::ofstream os(path, std::ios::out);

  // Actually run the algorithm
  auto cur_time = static_cast<unsigned long>(
      std::chrono::system_clock::now().time_since_epoch().count());
  img_renderer_t renderer(camera, camera_orientation, background, sch, cur_time,
                          50, delta_sampler(200));
  stdexec::sync_wait(renderer.render(bvh, img));
  write_ppm_img(os, img);
}
