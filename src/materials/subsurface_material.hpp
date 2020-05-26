#pragma once
#include "bssrdf.hpp"

class SubSurfaceMaterial {
 public:
  auto GetBxDF(const glm::vec3 &n) const { return BSSRDF{}; }
};