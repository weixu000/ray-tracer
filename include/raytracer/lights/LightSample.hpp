#pragma once
#include <glm/glm.hpp>

#include "raytracer/Ray.hpp"

/**
 * Local light information returned by Light::GenerateSample
 */
struct LightSample {
  glm::vec3 light;
  glm::vec3 radience, normal;
  float jacobian; // jacobian determinant

  Ray GetShadowRay(const glm::vec3 &incident) const {
    const auto d = light - incident;
    return Ray{incident + d * SHADOW_EPSILON, d};
  }
};