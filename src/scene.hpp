#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "lights/light.hpp"
#include "materials/composed_material.hpp"
#include "materials/ggx.hpp"
#include "materials/lambertian.hpp"
#include "materials/phong.hpp"
#include "soa.hpp"

struct MaterialRef {
  size_t type;
  size_t id;
};

/**
 * Hold primitives, lights, materials
 * Dispatch calls to different types of materials
 */
class Scene {
 public:
  std::vector<std::array<glm::vec3, 3>> triangles;
  std::vector<MaterialRef> triangle_materials;

  std::vector<glm::mat4> sphere_world_transforms;
  std::vector<glm::mat3> sphere_normal_transforms;
  std::vector<MaterialRef> sphere_materials;

  std::vector<std::unique_ptr<const Light>> lights;

  using Materials = SoA<ComposedMaterial<Lambertian, Phong>,
                        ComposedMaterial<Lambertian, GGXReflection>,
                        ComposedMaterial<GGXReflection, GGXRefraction>>;
  Materials materials;

  template <typename T, typename... Args>
  MaterialRef AddMaterial(Args... args) {
    const auto ref = materials.AddIfNew<T>(std::forward<Args>(args)...);
    return MaterialRef{ref.type, ref.id};
  }
};