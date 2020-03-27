#pragma once
#include <glm/glm.hpp>

#include "RayTracer/lights/Light.hpp"

class PointLight : public Light {
public:
  PointLight(const glm::vec3 &color, const glm::vec3 &position);

  std::optional<LightCast> GenerateLightRay(const glm::vec3 &position,
                                            const Scene &scene) const override;

private:
  glm::vec3 position_;
};