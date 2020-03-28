#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "raytracer/integrators/Integrator.hpp"

class SimpleIntegrator : public Integrator {
public:
  SimpleIntegrator(const Scene &scene, const Camera &camera, int max_depth = 5);

  std::vector<glm::u8vec3> Render() const override;

private:
  glm::vec3 Shade(const Ray &ray, int depth) const;

  int max_depth_;
};
