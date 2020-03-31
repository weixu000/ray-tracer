#pragma once
#include "raytracer/integrators/Integrator.hpp"

class AnalyticDirectIntegrator : public Integrator {
public:
  using Integrator::Integrator;

  std::vector<glm::u8vec3> Render() const override;

private:
  glm::vec3 Shade(const Ray &ray) const;
};
