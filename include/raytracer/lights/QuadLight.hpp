#pragma once
#include <glm/glm.hpp>

#include "raytracer/lights/Light.hpp"

class QuadLight : public Light {
public:
  QuadLight(const glm::vec3 &intensity, const glm::vec3 &v0,
            const glm::vec3 &e1, const glm::vec3 &e2);

  LightSample GenerateLightRay(const glm::vec3 &incident,
                               const glm::vec2 &uv) const override;

  std::optional<float> Intersect(const Ray &ray) const override;

  glm::vec3 GetIrradianceVector(const glm::vec3 &r) const;

private:
  glm::vec3 v_[4], v0_, e1_, e2_, normal_;
  float area_;
};
