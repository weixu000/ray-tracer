#pragma once

#include "shape.hpp"

class Triangle : public Shape {
 public:
  Triangle(const glm::mat4& transform, const glm::vec3& v0, const glm::vec3& v1,
           const glm::vec3& v2, const MaterialRef& mat);

  AABB GetAABB() const override {
    const auto v1 = v0_ + e1_, v2 = v0_ + e2_;
    return AABB{min(v0_, min(v1, v2)), max(v0_, max(v1, v2))};
  }

 private:
  std::optional<LocalInfo> Hit(const Ray& ray) const override;

  glm::vec3 v0_, e1_, e2_, n_;
};
