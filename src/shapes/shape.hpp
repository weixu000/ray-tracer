#pragma once
#include <glm/glm.hpp>
#include <optional>

#include "../materials/material_ref.hpp"
#include "../ray.hpp"
#include "aabb.hpp"
#include "local_info.hpp"
#include "ray_hit.hpp"

class Shape {
 public:
  explicit Shape(const MaterialRef &mat) : material(mat) {}

  virtual ~Shape() = default;

  std::optional<RayHit> HitMaterial(const Ray &ray) const {
    if (const auto hit = Hit(ray)) {
      return RayHit{hit->t, hit->n, material};
    } else {
      return std::nullopt;
    }
  }

  virtual AABB GetAABB() const = 0;

 private:
  virtual std::optional<LocalInfo> Hit(const Ray &ray) const = 0;

  MaterialRef material;
};