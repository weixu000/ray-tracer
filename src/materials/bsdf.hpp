#pragma once
#include <glm/glm.hpp>

struct BSDF {
  virtual ~BSDF() = default;

  virtual glm::vec3 Value(const glm::vec3 &w_i, const glm::vec3 &w_o) const = 0;

  virtual glm::vec3 Sample(const glm::vec3 &w_i) const = 0;

  virtual float Pdf(const glm::vec3 &w_i, const glm::vec3 &w_o) const = 0;

  virtual float Weight(const glm::vec3 &w_i) const = 0;
};