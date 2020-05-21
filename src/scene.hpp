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
#include "shapes/shape.hpp"
#include "soa.hpp"

/**
 * Hold primitives, lights, materials
 * Dispatch calls to different types of materials
 */
class Scene {
 public:
  std::vector<std::unique_ptr<const Shape>> shapes;
  std::vector<std::unique_ptr<const Light>> lights;

  using Materials = SoA<ComposedMaterial<Lambertian, Phong>,
                        ComposedMaterial<Lambertian, GGXReflection>>;
  Materials materials;

  template <typename T, typename... Args>
  MaterialRef AddMaterial(Args... args) {
    const auto ref = materials.AddIfNew<T>(std::forward<Args>(args)...);
    return MaterialRef{ref.type, ref.id};
  }

  template <typename... Args>
  glm::vec3 Brdf(const MaterialRef &material, Args &&... args) const {
    return Call(material, [&args...](const auto &m) {
      return m.Brdf(std::forward<Args>(args)...);
    });
  }

  template <typename... Args>
  glm::vec3 Sample(const MaterialRef &material, Args &&... args) const {
    return Call(material, [&args...](const auto &m) {
      return m.Sample(std::forward<Args>(args)...);
    });
  }

  template <typename... Args>
  float Pdf(const MaterialRef &material, Args &&... args) const {
    return Call(material, [&args...](const auto &m) {
      return m.Pdf(std::forward<Args>(args)...);
    });
  }

 private:
  template <typename F>
  auto Call(const MaterialRef &material, F f) const {
    return materials.Call(Materials::Ref{material.type, material.id}, f);
  }
};