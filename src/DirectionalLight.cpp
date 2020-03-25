#include "RayTracer/DirectionalLight.hpp"

#include "RayTracer/Scene.hpp"

DirectionalLight::DirectionalLight(const glm::vec3 &color,
                                   const glm::vec3 &direction)
    : Light(color), direction_(direction) {}

std::optional<LightCast>
DirectionalLight::GenerateLightRay(const glm::vec3 &position,
                                   const Scene &scene) const {
  using namespace glm;
  const auto d = normalize(direction_);
  const auto ray = Ray{position - d * SHADOW_EPSILON, -d};
  if (auto hit = scene.Trace(ray, 0); !hit) {
    return LightCast{color_, d};
  } else {
    return std::nullopt;
  }
}
