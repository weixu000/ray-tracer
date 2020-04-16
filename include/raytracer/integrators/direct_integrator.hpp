#pragma once

#include <raytracer/integrators/integrator.hpp>
#include <raytracer/samplers/sampler.hpp>

class DirectIntegrator : public Integrator {
public:
  using Integrator::Integrator;

  Sampler *sampler = nullptr;

private:
  glm::vec3 Shade(const Ray &ray) const override;
};
