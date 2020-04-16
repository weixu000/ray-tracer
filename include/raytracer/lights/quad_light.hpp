#pragma once

#include <raytracer/lights/light.hpp>

class QuadLight : public Light {
public:
  QuadLight(const glm::vec3 &radiance, const glm::vec3 &v0, const glm::vec3 &e1,
            const glm::vec3 &e2);

  LightSample GetSample(const glm::vec3 &x, const glm::vec2 &uv) const override;

  std::optional<LightEmission> Hit(const Ray &ray) const override;

  glm::vec3 GetRadiance() const { return radiance_; }

  glm::vec3 GetIrradianceVector(const glm::vec3 &r) const;

private:
  glm::vec3 radiance_;
  glm::vec3 v_[4], v0_, e1_, e2_, normal_;
  float area_;
};
