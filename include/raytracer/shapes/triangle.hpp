#pragma once

#include <raytracer/shapes/shape.hpp>

class Triangle : public Shape {
public:
  Triangle(const Material &mat, const glm::mat4 &transform, const glm::vec3 &v0,
           const glm::vec3 &v1, const glm::vec3 &v2);

private:
  std::optional<LocalInfo> Hit(const Ray &ray) const override;

  glm::vec3 v0_, e1_, e2_, n_;
};
