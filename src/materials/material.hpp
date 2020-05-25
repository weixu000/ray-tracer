#pragma once

#include <glm/glm.hpp>

struct BSDF {
  virtual ~BSDF() = default;

  virtual glm::vec3 Brdf(const glm::vec3 &w_i, const glm::vec3 &w_o) const = 0;

  virtual glm::vec3 Sample(const glm::vec3 &w_i) const = 0;

  virtual float Pdf(const glm::vec3 &w_i, const glm::vec3 &w_o) const = 0;

  virtual float Power(const glm::vec3 &w_i) const = 0;
};

class Material {
 public:
  virtual ~Material() = default;

  virtual const BSDF *GetBSDF(const glm::vec3 &n) const = 0;
};
