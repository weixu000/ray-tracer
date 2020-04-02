#pragma once
#include <glm/glm.hpp>

#include "raytracer/lights/Light.hpp"

class PointLight : public Light {
public:
  PointLight(const glm::vec3 &color, const glm::vec3 &position);

  LightSample GenerateLightRay(const glm::vec3 &incident,
                               const glm::vec2 &uv) const override;

  std::optional<float> Intersect(const Ray &ray) const override {
    return std::nullopt;
  }

  static inline glm::vec3 attenuation{1.f, 0.f, 0.f};

private:
  glm::vec3 position_;
};