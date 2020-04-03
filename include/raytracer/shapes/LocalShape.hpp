#pragma once

#include <glm/glm.hpp>

#include "raytracer/shapes/Shape.hpp"

// Handles local-world transform
class LocalShape : public Shape {
public:
  explicit LocalShape(const glm::mat4 &transform);

  std::optional<LocalInfo> Hit(const Ray &ray) const override;

protected:
  glm::mat4 world_{};
  glm::mat4 local_{};
  glm::mat3 normal_world_{};

private:
  // Hit in local space
  virtual std::optional<LocalInfo> HitLocal(const Ray &ray) const = 0;
};
