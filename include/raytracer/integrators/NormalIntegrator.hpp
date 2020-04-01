#pragma once

#include "raytracer/integrators/Integrator.hpp"

// Normal shading
class NormalIntegrator : public Integrator {
public:
  using Integrator::Integrator;

private:
  glm::vec3 Shade(const Ray &ray) const override;
};
