#pragma once

#include <glm/glm.hpp>

#include "bsdf.hpp"

class Material {
 public:
  virtual ~Material() = default;

  virtual const BSDF *GetBSDF(const glm::vec3 &n) const = 0;
};
