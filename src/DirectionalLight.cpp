#include "raytracer/lights/DirectionalLight.hpp"

#include "raytracer/Ray.hpp"
#include "raytracer/Scene.hpp"

using namespace glm;

DirectionalLight::DirectionalLight(const glm::vec3 &color,
                                   const glm::vec3 &direction)
    : Light(color), direction_(normalize(direction)) {}

LightRay DirectionalLight::GenerateLightRay(const glm::vec3 &p) const {
  return LightRay{p + direction_ * SHADOW_EPSILON, direction_, color_, FLT_MAX};
}
