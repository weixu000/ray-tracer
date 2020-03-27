#pragma once
#include <glm/glm.hpp>

#include "RayTracer/Geometry.hpp"

class Sphere : public Geometry {
public:
  Sphere() = default;

  Sphere(const glm::mat4 &transform, const glm::vec3 &position, float radius);

  std::optional<RayHit> Intersect(const Ray &ray) const override;
};