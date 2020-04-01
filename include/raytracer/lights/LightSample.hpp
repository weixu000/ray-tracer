#pragma once
#include <glm/glm.hpp>

#include "raytracer/Ray.hpp"

struct LightSample {
  glm::vec3 light, incident;
  glm::vec3 radience, normal;
  float area;

  Ray GetShadowRay() const;
};