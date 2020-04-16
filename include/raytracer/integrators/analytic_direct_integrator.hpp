#pragma once

#include <raytracer/integrators/integrator.hpp>

class AnalyticDirectIntegrator : public Integrator {
public:
  using Integrator::Integrator;

private:
  glm::vec3 Shade(const Ray &ray) const override;
};
