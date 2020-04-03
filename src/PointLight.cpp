#include "raytracer/lights/PointLight.hpp"

#include "raytracer/Ray.hpp"
#include "raytracer/Scene.hpp"

PointLight::PointLight(const glm::vec3 &color, const glm::vec3 &position)
    : Light(color), position_(position) {}

LightSample PointLight::GenerateSample(const glm::vec3 &incident,
                                       const glm::vec2 &uv) const {
  using namespace glm;
  const auto d = position_ - incident;
  const auto d_l = length(d);
  const auto d_p = glm::vec3(1.f, d_l, d_l * d_l);
  const auto attenuation = dot(PointLight::attenuation, d_p);
  return LightSample{position_, intensity / attenuation};
}
