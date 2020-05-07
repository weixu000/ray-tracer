#pragma once
#include <glm/glm.hpp>
#include <optional>

#include "../aabb.hpp"
#include "../material.hpp"
#include "../ray.hpp"
#include "local_info.hpp"
#include "ray_hit.hpp"

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

  virtual AABB GetAABB() const = 0;

private:
  virtual std::optional<LocalInfo> Hit(const Ray &ray) const = 0;

  Material material;
};