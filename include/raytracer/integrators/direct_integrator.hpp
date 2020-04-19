#pragma once

#include <memory>

#include <raytracer/integrators/integrator.hpp>
#include <raytracer/samplers/sampler.hpp>

class DirectIntegrator : public Integrator {
public:
  using Integrator::Integrator;

  std::unique_ptr<Sampler> sampler;

private:
  glm::vec3 ShadePixel(const glm::vec2 &pixel) const override;
};
