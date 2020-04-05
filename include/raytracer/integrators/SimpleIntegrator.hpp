#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "raytracer/integrators/Integrator.hpp"

/**
 * Recursive raytracing
 */
class SimpleIntegrator : public Integrator {
public:
  using Integrator::Integrator;

  int max_depth_ = 5;

private:
  glm::vec3 Shade(const Ray &ray) const override {
    return Shade(ray, max_depth_);
  }

  glm::vec3 Shade(const Ray &ray, int depth) const;
};
