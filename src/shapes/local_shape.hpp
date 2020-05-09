#pragma once

#include "shape.hpp"

/**
 * Special base class for shapes that handles local-world transform
 */
class LocalShape : public Shape {
 public:
  LocalShape(const glm::mat4& transform, const MaterialRef& mat);

 protected:
  glm::mat4 world_{};
  glm::mat4 local_{};
  glm::mat3 normal_world_{};

 private:
  std::optional<LocalInfo> Hit(const Ray& ray) const override;

  virtual std::optional<LocalInfo> HitLocal(const Ray& ray) const = 0;
};
