#pragma once

#include <glm/glm.hpp>

class Material {
 public:
  Material() = default;

  virtual ~Material() = default;

  virtual glm::vec3 Brdf(const glm::vec3 &n, const glm::vec3 &w_i,
                         const glm::vec3 &w_o) const = 0;

  virtual glm::vec3 Sample(const glm::vec3 &n, const glm::vec3 &w_i) const = 0;

  virtual float Pdf(const glm::vec3 &n, const glm::vec3 &w_i,
                    const glm::vec3 &w_o) const = 0;

  virtual float Power(const glm::vec3 &n, const glm::vec3 &w_i) const = 0;
};
