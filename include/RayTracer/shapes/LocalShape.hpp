#pragma once

#include <glm/glm.hpp>

#include "RayTracer/shapes/Shape.hpp"

// Handles local-world transform
class LocalShape : public Shape {
public:
  explicit LocalShape(const glm::mat4 &transform);

protected:
  Ray WorldToLocal(const Ray &ray) const;
  glm::vec3 NormalToWorld(const glm::vec3 &n) const;

  const glm::mat4 world_{};
  const glm::mat4 local_{};
  const glm::mat3 normal_world_{};
};
