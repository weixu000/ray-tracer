#pragma once
#include <glm/glm.hpp>

#include "RayTracer/Geometry.h"

class Sphere : public Geometry {
public:
  Sphere() = default;

  Sphere(const glm::mat4 &transform, const Material &material,
         const glm::vec3 &position, float radius);

private:
  std::optional<RayHit> IntersectLocal(const Ray &ray) const override;

  glm::vec3 position_;
  float radius_;
};