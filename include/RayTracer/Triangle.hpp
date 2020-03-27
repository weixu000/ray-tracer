#pragma once

#include <glm/glm.hpp>

#include "RayTracer/Geometry.hpp"

class Triangle : public Geometry {
public:
  Triangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2);

  std::optional<RayHit> Intersect(const Ray &ray) const override;

  AABB GetWorldAABB() const override { return aabb; }

private:
  const glm::vec3 v0, e1, e2, normal;
  const AABB aabb;
};
