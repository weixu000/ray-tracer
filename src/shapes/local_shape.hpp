#pragma once

#include "shape.hpp"

/**
 * Special base class for shapes that handles local-world transform
 */
class LocalShape : public Shape {
 public:
  LocalShape(const glm::mat4& transform, const MaterialRef& mat)
      : Shape(mat),
        world_(transform),
        local_(inverse(transform)),
        normal_world_(glm::inverse(glm::transpose(glm::mat3(transform)))) {}

 protected:
  glm::mat4 world_{};
  glm::mat4 local_{};
  glm::mat3 normal_world_{};

 private:
  std::optional<LocalInfo> Hit(const Ray& ray) const override {
    using namespace glm;
    const auto local_ray =
        Ray{local_ * vec4(ray.origin, 1.f), local_ * vec4(ray.direction, 0.f)};
    if (const auto local = HitLocal(local_ray)) {
      return LocalInfo{local->t, normalize(normal_world_ * local->n)};
    } else {
      return std::nullopt;
    }
  }

  virtual std::optional<LocalInfo> HitLocal(const Ray& ray) const = 0;
};
