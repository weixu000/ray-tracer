#pragma once

#include "local_shape.hpp"

class Sphere : public LocalShape {
 public:
  Sphere(const glm::vec3& position, float radius, const glm::mat4& transform,
         const MaterialRef& mat);

  AABB GetAABB() const override;

 private:
  std::optional<LocalInfo> HitLocal(const Ray& ray) const override;
};