#pragma once
#include <glm/glm.hpp>

#include "raytracer/lights/Light.hpp"

class DirectionalLight : public Light {
public:
  DirectionalLight(const glm::vec3 &color, const glm::vec3 &direction);

  LightRay GenerateLightRay(const glm::vec3 &p) const override;

private:
  glm::vec3 direction_; // Direction towards the light source
};