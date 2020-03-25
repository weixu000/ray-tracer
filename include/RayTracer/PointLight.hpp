#pragma once
#include <glm/glm.hpp>

#include "RayTracer/Light.hpp"

class PointLight : public Light {
public:
  glm::vec3 position_;

  std::optional<LightCast> GenerateLightRay(const glm::vec3 &position,
                                            const Scene &scene) const override;
};