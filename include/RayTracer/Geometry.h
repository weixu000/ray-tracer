#pragma once
#include <optional>

#include <glm/glm.hpp>

#include "RayTracer/Material.h"
#include "RayTracer/Ray.h"

class Geometry {
public:
  Geometry() = default;

  Geometry(const glm::mat4 &transform, const Material &material);

  std::optional<RayHit> Intersect(const Ray &ray) const;

  const Material &GetMaterial() const;

private:
  virtual std::optional<RayHit> IntersectLocal(const Ray &ray) const = 0;

  glm::mat4 world_to_local_;
  Material material_;
};