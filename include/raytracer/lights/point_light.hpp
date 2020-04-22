#pragma once

#include <glm/gtx/norm.hpp>

#include <raytracer/lights/delta_light.hpp>

class PointLight : public DeltaLight {
public:
  PointLight(const glm::vec3 &intensity, const glm::vec3 &position)
      : intensity_(intensity), position_(position) {}

  LightRay GetRay(const glm::vec3 &x) const override {
    using namespace glm;
    const auto d = position_ - x;
    const auto dd = length(d);
    // return LightRay{d, intensity_, 1.f};  // for hw1
    return LightRay{d / dd, intensity_ / (d * d), dd};  // physically correct
  }

private:
  glm::vec3 intensity_;
  glm::vec3 position_;
};