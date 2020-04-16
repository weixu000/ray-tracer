#pragma once

#include <raytracer/shapes/local_shape.hpp>

class Sphere : public LocalShape {
public:
  Sphere(const Material &mat, const glm::mat4 &transform,
         const glm::vec3 &position, float radius);

private:
  std::optional<LocalInfo> HitLocal(const Ray &ray) const override;
};