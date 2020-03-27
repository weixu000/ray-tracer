#pragma once
#include <glm/glm.hpp>

#include "RayTracer/geometries/LocalGeometry.hpp"

class Sphere : public LocalGeometry {
public:
  Sphere() = default;

  Sphere(const glm::mat4 &transform, const glm::vec3 &position, float radius);

  std::optional<RayHit> Intersect(const Ray &ray) const override;

  AABB GetWorldAABB() const override;
};