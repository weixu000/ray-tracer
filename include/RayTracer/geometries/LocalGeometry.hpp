#pragma once

#include <glm/glm.hpp>

#include "RayTracer/geometries/Geometry.hpp"

// Handles local-world transform
class LocalGeometry : public Geometry {
public:
  explicit LocalGeometry(const glm::mat4 &transform);

protected:
  Ray WorldToLocal(const Ray &ray) const;
  glm::vec3 NormalToWorld(const glm::vec3 &n) const;

  const glm::mat4 world_{};
  const glm::mat4 local_{};
  const glm::mat3 normal_world_{};
};
