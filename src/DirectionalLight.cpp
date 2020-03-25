#include "RayTracer/DirectionalLight.hpp"

#include "RayTracer/Scene.hpp"

std::optional<LightCast>
DirectionalLight::GenerateLightRay(const glm::vec3 &position,
                                   const Scene &scene) const {
  using namespace glm;
  const auto d = normalize(direction);
  const auto ray = Ray{position - d * SHADOW_EPSILON, -d};
  if (auto hit = scene.Trace(ray, 0); !hit) {
    return LightCast{color, d};
  } else {
    return std::nullopt;
  }
}