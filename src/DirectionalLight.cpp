#include "RayTracer/lights/DirectionalLight.hpp"

#include "RayTracer/Ray.hpp"
#include "RayTracer/Scene.hpp"

using namespace glm;

DirectionalLight::DirectionalLight(const glm::vec3 &color,
                                   const glm::vec3 &direction)
    : Light(color), direction_(normalize(direction)) {}

std::optional<LightCast>
DirectionalLight::GenerateLightRay(const glm::vec3 &position,
                                   const Scene &scene) const {
  const auto d = direction_;
  const auto ray = Ray{position + d * SHADOW_EPSILON, d};
  if (!scene.Trace(ray)) {
    return LightCast{color_, -d};
  } else {
    return std::nullopt;
  }
}
