#pragma once
#include <glm/glm.hpp>

#include "raytracer/lights/Light.hpp"

class PointLight : public Light {
public:
  PointLight(const glm::vec3 &color, const glm::vec3 &position);

  LightRay GenerateLightRay(const glm::vec3 &incident, float u,
                            float v) const override;

private:
  glm::vec3 position_;
};