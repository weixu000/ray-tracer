#pragma once

#include "local_shape.hpp"

class Sphere : public LocalShape {
public:
  Sphere(const Material &mat, const glm::mat4 &transform,
         const glm::vec3 &position, float radius);

  AABB GetAABB() const override;

private:
  std::optional<LocalInfo> HitLocal(const Ray &ray) const override;
};