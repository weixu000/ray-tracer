#pragma once

#include <raytracer/lights/light.hpp>

class PointLight : public Light {
public:
  PointLight(const glm::vec3 &color, const glm::vec3 &position)
      : Light(color), position_(position) {}

  LightSample GenerateSample(const glm::vec3 &incident,
                             const glm::vec2 &uv) const override {
    using namespace glm;
    const auto d = position_ - incident;
    const auto d_l = length(d);
    const auto d_p = glm::vec3(1.f, d_l, d_l * d_l);
    const auto attenuation = dot(PointLight::attenuation, d_p);
    return LightSample{position_, intensity / attenuation};
  }

  std::optional<float> Hit(const Ray &ray) const override {
    return std::nullopt;
  }

  static inline glm::vec3 attenuation{1.f, 0.f, 0.f};

private:
  glm::vec3 position_;
};