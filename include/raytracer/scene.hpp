#pragma once

#include <memory>
#include <string>
#include <vector>

#include <raytracer/lights/delta_light.hpp>
#include <raytracer/lights/light.hpp>
#include <raytracer/primitives/primitive_group.hpp>

/**
 * Hold primitives and lights
 */
class Scene {
public:
  PrimitiveGroup group;
  std::vector<std::unique_ptr<Light>> lights;
  std::vector<std::unique_ptr<DeltaLight>> delta_lights;

  std::optional<RayHit> Trace(const Ray &ray) const { return group.Hit(ray); }

  std::optional<LightEmission> TraceLight(const Ray &ray) const {
    std::optional<LightEmission> ret;
    for (const auto &light : lights) {
      if (const auto hit = light->Hit(ray);
          hit && (!ret || hit->distance < ret->distance)) {
        ret = hit;
      }
    }
    return ret;
  }
};