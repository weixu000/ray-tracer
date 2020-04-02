#pragma once
#include "raytracer/integrators/Integrator.hpp"
#include "raytracer/samplers/Sampler.hpp"

class DirectIntegrator : public Integrator {
public:
  using Integrator::Integrator;

  Sampler *sampler = nullptr;

private:
  glm::vec3 Shade(const Ray &ray) const override;
};
