#pragma once

#include <glm/glm.hpp>

#include "raytracer/shapes/Shape.hpp"

class Triangle : public Shape {
public:
  Triangle(const Material &mat, const glm::mat4 &transform, const glm::vec3 &v0,
           const glm::vec3 &v1, const glm::vec3 &v2);

  std::optional<LocalInfo> Hit(const Ray &ray) const override;

private:
  glm::vec3 v0_, e1_, e2_, n_;
};
