#include "raytracer/lights/LightSample.hpp"

Ray LightSample::GetShadowRay() const {
  const auto d = light - incident;
  return Ray{incident + d * SHADOW_EPSILON, d};
}
