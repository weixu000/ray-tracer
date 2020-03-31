#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "raytracer/integrators/Integrator.hpp"

class SimpleIntegrator : public Integrator {
public:
  using Integrator::Integrator;

  std::vector<glm::u8vec3> Render() const override;

  int max_depth_ = 5;

private:
  glm::vec3 Shade(const Ray &ray, int depth) const;
};
