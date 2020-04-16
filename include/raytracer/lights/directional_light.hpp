#pragma once

#include <raytracer/lights/delta_light.hpp>

class DirectionalLight : public DeltaLight {
public:
  DirectionalLight(const glm::vec3 &irradiance, const glm::vec3 &direction)
      : irradiance_(irradiance), direction_(glm::normalize(direction)) {}

  LightRay GetRay(const glm::vec3 &x) const override {
    return LightRay{direction_, irradiance_, FLT_MAX};
  }

private:
  glm::vec3 irradiance_;
  glm::vec3 direction_;  // Direction towards the light source
};