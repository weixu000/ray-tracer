#pragma once

#include <raytracer/integrators/integrator.hpp>
#include <raytracer/samplers/hemisphere_sampler.hpp>
#include <raytracer/samplers/square_sampler.hpp>

class PathIntegrator : public Integrator {
public:
  using Integrator::Integrator;

  int max_depth_ = 5;

  int num_sample_ = 1;

private:
  glm::vec3 ShadePixel(const glm::vec2 &pixel) const override;

  glm::vec3 Sample(const Ray &ray, int depth) const;

  HemisphereSampler sphere_sampler_;
  SquareSampler pixel_sampler_;
};
