#pragma once
#include "bssrdf.hpp"

class SubSurfaceMaterial {
 public:
  SubSurfaceMaterial(float albedo, float ior) : albedo_(albedo), ior_(ior) {}

  auto GetBxDF(const glm::vec3 &n) const { return BSSRDF{albedo_, ior_}; }

 private:
  float albedo_, ior_;
};