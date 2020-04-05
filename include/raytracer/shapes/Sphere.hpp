#pragma once
#include <glm/glm.hpp>

#include "raytracer/shapes/LocalShape.hpp"

class Sphere : public LocalShape {
public:
  Sphere(const glm::mat4 &transform, const glm::vec3 &position, float radius);

  AABB GetAABB() const override;

private:
  std::optional<LocalInfo> HitLocal(const Ray &ray) const override;
};