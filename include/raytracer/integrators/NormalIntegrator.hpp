#pragma once

#include "raytracer/integrators/Integrator.hpp"

// Normal shading
class NormalIntegrator : public Integrator {
public:
  using Integrator::Integrator;

  std::vector<glm::u8vec3> Render() const override;
};
