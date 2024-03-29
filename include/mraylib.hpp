#include "angle.hpp"
#include "aspect_ratio.hpp"
#include "bound.hpp"
#include "camera/camera.hpp"
#include "camera/camera_orientation.hpp"
#include "camera/concepts.hpp"
#include "color.hpp"
#include "dimension.hpp"
#include "direction.hpp"
#include "equation.hpp"
#include "generator/concepts.hpp"
#include "generator/direction_generator.hpp"
#include "generator/generator_view.hpp"
#include "generator/random_double_generator.hpp"
#include "generator/thread_local_random_double_generator.hpp"
#include "generator/unit_disk_generator.hpp"
#include "hit_info.hpp"
#include "image/concepts.hpp"
#include "image/in_memory_image.hpp"
#include "image/ppm/ppm_utils.hpp"
#include "image/solid_color_image.hpp"
#include "image_renderer.hpp"
#include "interval.hpp"
#include "light.hpp"
#include "materials/concept.hpp"
#include "materials/dielectric.hpp"
#include "materials/diffuse_light.hpp"
#include "materials/emit_info.hpp"
#include "materials/lambertian.hpp"
#include "materials/material_context.hpp"
#include "materials/metal.hpp"
#include "materials/scatter_info.hpp"
#include "normal.hpp"
#include "pixel_sampler/concepts.hpp"
#include "pixel_sampler/delta_sampler.hpp"
#include "pixel_sampler/identity_sampler.hpp"
#include "pixel_sampler/sampler_args.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "rotation.hpp"
#include "scale_2d.hpp"
#include "scene.hpp"
#include "scene_objects/any_scene_object.hpp"
#include "scene_objects/bvh.hpp"
#include "scene_objects/concepts.hpp"
#include "scene_objects/object_ref.hpp"
#include "scene_objects/rotate_object.hpp"
#include "scene_objects/scene_object_range.hpp"
#include "scene_objects/shapes/concepts.hpp"
#include "scene_objects/shapes/quad.hpp"
#include "scene_objects/shapes/shape_object.hpp"
#include "scene_objects/shapes/sphere.hpp"
#include "scene_objects/traits.hpp"
#include "scene_objects/translate_object.hpp"
#include "schedulers/concepts.hpp"
#include "schedulers/inline_scheduler.hpp"
#include "schedulers/libdispatch_queue.hpp"
#include "schedulers/type_traits.hpp"
#include "std/algorithm.hpp"
#include "std/btree.hpp"
#include "std/concepts.hpp"
#include "std/function_ref.hpp"
#include "std/functional.hpp"
#include "std/hierarchy_tree.hpp"
#include "std/optional.hpp"
#include "std/ranges.hpp"
#include "textures/checker_texture.hpp"
#include "textures/concepts.hpp"
#include "textures/image_texture.hpp"
#include "textures/perlin_texture.hpp"
#include "textures/solid_color.hpp"
#include "utils/double_utils.hpp"
#include "vector.hpp"
