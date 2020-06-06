#pragma once

#include <memory>
#include <variant>
#include <vector>

#include "geometry.hpp"
#include "lights/light.hpp"
#include "materials/ggx.hpp"
#include "materials/phong.hpp"
#include "materials/subsurface_material.hpp"

using Material = std::variant<GGXRefraction, Phong, SubSurfaceMaterial>;

/**
 * Hold primitives, lights, materials
 * Dispatch calls to different types of materials
 */
class Scene {
 public:
  std::vector<Geometry> geometries;
  std::vector<Material> materials;
  std::vector<std::unique_ptr<const Light>> lights;

  template <typename T, typename... Args>
  MaterialRef AddMaterial(Args... args) {
    materials.emplace_back(T{std::forward<Args>(args)...});
    return materials.size() - 1;
  }
};