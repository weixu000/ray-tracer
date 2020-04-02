#include "raytracer/lights/DirectionalLight.hpp"

#include "raytracer/Ray.hpp"

using namespace glm;

DirectionalLight::DirectionalLight(const glm::vec3 &color,
                                   const glm::vec3 &direction)
    : Light(color), direction_(normalize(direction)) {}

LightSample DirectionalLight::GenerateLightRay(const glm::vec3 &incident,
                                               const glm::vec2 &uv) const {
  return LightSample{incident + direction_ * distance_, incident, intensity};
}
