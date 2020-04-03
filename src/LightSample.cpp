#include "raytracer/lights/LightSample.hpp"

Ray LightSample::GetShadowRay(const glm::vec3 &incident) const {
  const auto d = light - incident;
  return Ray{incident + d * SHADOW_EPSILON, d};
}
