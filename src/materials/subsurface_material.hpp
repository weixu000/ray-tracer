#pragma once
#include "bssrdf.hpp"

class SubSurfaceMaterial {
 public:
  SubSurfaceMaterial(float albedo, float ior, float d)
      : albedo_(albedo), ior_(ior), d_(d) {}

  auto GetBxDF(const glm::vec3 &n) const { return BSSRDF{albedo_, ior_, d_}; }

 private:
  float albedo_, ior_, d_;
};