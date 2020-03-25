#pragma once
#include <glm/glm.hpp>

#include "RayTracer/Light.hpp"

class DirectionalLight : public Light {
public:
  DirectionalLight(const glm::vec3 &color, const glm::vec3 &direction);

  std::optional<LightCast> GenerateLightRay(const glm::vec3 &position,
                                            const Scene &scene) const override;

private:
  glm::vec3 direction_;
};