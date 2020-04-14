#pragma once
#include <optional>

#include <glm/glm.hpp>

#include "raytracer/Material.hpp"
#include "raytracer/Ray.hpp"
#include "raytracer/primitives/AABB.hpp"
#include "raytracer/shapes/LocalInfo.hpp"
#include "raytracer/shapes/RayHit.hpp"

class Shape {
public:
  explicit Shape(const Material &mat) : material(mat) {}

  virtual ~Shape() = default;

  virtual std::optional<LocalInfo> Hit(const Ray &ray) const = 0;

  std::optional<RayHit> HitMaterial(const Ray &ray) const {
    if (const auto hit = Hit(ray)) {
      return RayHit{hit->t, hit->normal, &material, material.GetBRDF(*hit)};
    } else {
      return std::nullopt;
    }
  }

  AABB GetAABB() const { return aabb_; }

protected:
  AABB aabb_;

private:
  Material material;
};