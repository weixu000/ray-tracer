#pragma once

#include <memory>
#include <optional>
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
  std::vector<std::unique_ptr<const Light>> lights;
  std::vector<std::unique_ptr<const DeltaLight>> delta_lights;

  std::optional<RayHit> TraceShapes(const Ray &ray) const {
    return group.Hit(ray);
  }

  std::optional<LightEmission> TraceLights(const Ray &ray) const {
    std::optional<LightEmission> ret;
    for (const auto &light : lights) {
      if (const auto hit = light->Hit(ray);
          hit && (!ret || hit->distance < ret->distance)) {
        ret = hit;
      }
    }
    return ret;
  }

  template <typename T1, typename T2>
  glm::vec3 Trace(const Ray &ray, T1 if_light, T2 if_shape,
                  const glm::vec3 &default_val = glm::vec3(0.f)) const {
    const auto light_hit = TraceLights(ray);
    const auto shape_hit = TraceShapes(ray);
    if (shape_hit && (!light_hit || shape_hit->t < light_hit->distance)) {
      return if_shape(*shape_hit);
    } else if (light_hit) {
      return if_light(*light_hit);
    } else {
      return default_val;
    }
  }
};