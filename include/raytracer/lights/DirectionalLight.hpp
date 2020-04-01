#pragma once
#include <glm/glm.hpp>

#include "raytracer/lights/Light.hpp"

class DirectionalLight : public Light {
public:
  DirectionalLight(const glm::vec3 &color, const glm::vec3 &direction);

  LightSample GenerateLightRay(const glm::vec3 &incident, float u,
                               float v) const override;

private:
  glm::vec3 direction_; // Direction towards the light source

  static inline const float distance_ = 1000.f; // How far away the light is
};