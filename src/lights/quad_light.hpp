#pragma once

#include "light.hpp"

class QuadLight : public Light {
 public:
  QuadLight(const glm::vec3 &radiance, const glm::vec3 &v0, const glm::vec3 &e1,
            const glm::vec3 &e2);

  glm::vec3 Sample() const override;

  std::optional<Emission> Hit(const Ray &ray) const override;

 private:
  glm::vec3 radiance_;
  glm::vec3 v_[4], v0_, e1_, e2_, normal_;
  float area_;
};
