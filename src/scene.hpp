#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "lights/light.hpp"
#include "materials/ggx.hpp"
#include "materials/phong.hpp"
#include "shapes/shape.hpp"

/**
 * Hold primitives and lights
 */
class Scene {
 public:
  std::vector<std::unique_ptr<const Shape>> shapes;
  std::vector<std::unique_ptr<const Light>> lights;
  std::vector<Phong> phong;
  std::vector<GGX> ggx;

  template <typename... Args>
  glm::vec3 Brdf(const MaterialRef &material, Args &&... args) const {
    switch (material.type) {
      case BRDF::Phong:
        return phong[material.id].Brdf(std::forward<Args>(args)...);
      case BRDF::GGX:
        return ggx[material.id].Brdf(std::forward<Args>(args)...);
      default:
        throw;
    }
  }

  template <typename... Args>
  glm::vec3 Sample(const MaterialRef &material, Args &&... args) const {
    switch (material.type) {
      case BRDF::Phong:
        return phong[material.id].Sample(std::forward<Args>(args)...);
      case BRDF::GGX:
        return ggx[material.id].Sample(std::forward<Args>(args)...);

      default:
        throw;
    }
  }

  template <typename... Args>
  float Pdf(const MaterialRef &material, Args &&... args) const {
    switch (material.type) {
      case BRDF::Phong:
        return phong[material.id].Pdf(std::forward<Args>(args)...);
      case BRDF::GGX:
        return ggx[material.id].Pdf(std::forward<Args>(args)...);
      default:
        throw;
    }
  }
};