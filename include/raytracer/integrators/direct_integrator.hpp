#pragma once

#include <memory>

#include <raytracer/integrators/integrator.hpp>
#include <raytracer/samplers/multisampler.hpp>

class DirectIntegrator : public Integrator {
public:
  using Integrator::Integrator;

  std::unique_ptr<const Multisampler> sampler_;

protected:
  glm::vec3 LightDirect(const glm::vec3& x, const glm::vec3& n,
                        const glm::vec3& w_o, const BRDF& brdf) const;

private:
  glm::vec3 ShadePixel(const glm::vec2& pixel) const override;
};
