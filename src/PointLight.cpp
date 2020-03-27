#include "RayTracer/lights/PointLight.hpp"

#include "RayTracer/Ray.hpp"
#include "RayTracer/Scene.hpp"

PointLight::PointLight(const glm::vec3 &color, const glm::vec3 &position)
    : Light(color), position_(position) {}

std::optional<LightCast>
PointLight::GenerateLightRay(const glm::vec3 &position,
                             const Scene &scene) const {
  using namespace glm;
  const auto d = position - position_;
  const auto ray = Ray{position_, d};
  if (auto hit = scene.Trace(ray); !hit || hit->t + SHADOW_EPSILON > 1.f) {
    const auto dl = length(d);
    const auto d_p = glm::vec3(1.f, dl, dl * dl);
    const auto attenuation = dot(Light::attenuation, d_p);
    return LightCast{color_ / attenuation, d};
  } else {
    return std::nullopt;
  }
}
