#pragma once
#include <glm/glm.hpp>

#include "raytracer/Ray.hpp"

struct LightSample {
  glm::vec3 light;
  glm::vec3 radience, normal;
  float jacobian;

  Ray GetShadowRay(const glm::vec3 &incident) const;
};