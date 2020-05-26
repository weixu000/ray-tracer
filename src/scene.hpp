#pragma once

#include <memory>
#include <variant>
#include <vector>

#include "lights/light.hpp"
#include "materials/ggx.hpp"
#include "materials/phong.hpp"
#include "materials/subsurface_material.hpp"

using Material =
    std::variant<GGXReflection, GGXRefraction, Phong, SubSurfaceMaterial>;

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

  std::vector<Material> materials;

  template <typename T, typename... Args>
  MaterialRef AddMaterial(Args... args) {
    materials.emplace_back(T{std::forward<Args>(args)...});
    return materials.size() - 1;
  }
};