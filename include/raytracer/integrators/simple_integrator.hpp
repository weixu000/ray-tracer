#pragma once

#include <vector>

#include <raytracer/integrators/integrator.hpp>

/**
 * Recursive raytracing
 */
class SimpleIntegrator : public Integrator {
public:
  using Integrator::Integrator;

  int max_depth_ = 5;

private:
  glm::vec3 ShadePixel(const glm::vec2 &pixel) const override {
    return Shade(camera_.GenerateEyeRay(pixel + .5f), max_depth_);
  }

  glm::vec3 Shade(const Ray &ray, int depth) const;
};
