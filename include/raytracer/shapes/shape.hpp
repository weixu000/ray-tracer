#pragma once
#include <optional>

#include <glm/glm.hpp>

#include <raytracer/material.hpp>
#include <raytracer/primitives/aabb.hpp>
#include <raytracer/ray.hpp>
#include <raytracer/shapes/local_info.hpp>
#include <raytracer/shapes/ray_hit.hpp>

class Shape {
public:
  explicit Shape(const Material &mat) : material(mat) {}

  virtual ~Shape() = default;

  std::optional<RayHit> HitMaterial(const Ray &ray) const {
    if (const auto hit = Hit(ray)) {
      return RayHit{hit->t, hit->normal, &material};
    } else {
      return std::nullopt;
    }
  }

  AABB GetAABB() const { return aabb_; }

protected:
  AABB aabb_;

private:
  virtual std::optional<LocalInfo> Hit(const Ray &ray) const = 0;

  Material material;
};