#pragma once

#include "raytracer/integrators/Integrator.hpp"

/**
 * Normal shading
 */
class NormalIntegrator : public Integrator {
public:
  using Integrator::Integrator;

private:
  glm::vec3 Shade(const Ray &ray) const override {
    using namespace glm;

    auto color = vec3(0.f);
    if (const auto hit = scene_.Trace(ray)) {
      color = (normalize(hit->normal) + 1.f) / 2.f;
    }
    return color;
  }
};
