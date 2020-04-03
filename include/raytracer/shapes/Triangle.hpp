#pragma once

#include <glm/glm.hpp>

#include "raytracer/shapes/Shape.hpp"

class Triangle : public Shape {
public:
  Triangle(const glm::mat4 &transform, const glm::vec3 &v0, const glm::vec3 &v1,
           const glm::vec3 &v2);

  std::optional<RayHit> Hit(const Ray &ray) const override;

  AABB GetAABB() const override { return aabb_; }

private:
  glm::vec3 v0_, e1_, e2_, n_;
  AABB aabb_;
};
