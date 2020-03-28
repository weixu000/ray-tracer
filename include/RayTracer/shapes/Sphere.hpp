#pragma once
#include <glm/glm.hpp>

#include "RayTracer/shapes/LocalShape.hpp"

class Sphere : public LocalShape {
public:
  Sphere() = default;

  Sphere(const glm::mat4 &transform, const glm::vec3 &position, float radius);

  std::optional<RayHit> Hit(const Ray &ray) const override;

  AABB GetWorldAABB() const override;
};