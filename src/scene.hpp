#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "lights/light.hpp"
#include "materials/material.hpp"

/**
 * Hold primitives, lights, materials
 * Dispatch calls to different types of materials
 */
class Scene {
 public:
  std::vector<std::array<glm::vec3, 3>> triangles;
  std::vector<const Material*> triangle_materials;

  std::vector<glm::mat4> sphere_world_transforms;
  std::vector<glm::mat3> sphere_normal_transforms;
  std::vector<const Material*> sphere_materials;

  std::vector<std::unique_ptr<const Light>> lights;

  std::vector<std::unique_ptr<const Material>> materials;

  template <typename T, typename... Args>
  const Material* AddMaterial(Args... args) {
    materials.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    return materials.back().get();
  }
};