#pragma once

#include "integrator.hpp"

/**
 * Normal shading
 */
class NormalIntegrator : public Integrator {
 public:
  using Integrator::Integrator;

 private:
  glm::vec3 ShadePixel(const glm::vec2 &pixel) const override {
    using namespace glm;

    const auto ray = camera_.GenerateEyeRay(pixel + .5f);

    if (const auto hit = scene_.TraceShapes(ray)) {
      return (hit->normal + 1.f) / 2.f;
    } else {
      return vec3(0.f);
    }
  }
};
