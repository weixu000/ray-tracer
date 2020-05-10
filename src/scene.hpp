#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "bvh.hpp"
#include "lights/light.hpp"
#include "materials/ggx.hpp"
#include "materials/phong.hpp"

/**
 * Hold primitives and lights
 */
class Scene {
 public:
  BVH group;
  std::vector<std::unique_ptr<const Light>> lights;
  std::vector<Phong> phong;
  std::vector<GGX> ggx;

  template <typename... Args>
  glm::vec3 Brdf(const MaterialRef &material, Args &&... args) const {
    switch (material.type) {
      case BRDF::Phong:
        return phong[material.id].Brdf(std::forward<Args>(args)...);
      case BRDF::GGX:
        return ggx[material.id].Brdf(std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  std::tuple<glm::vec3, float> SampleBrdf(const MaterialRef &material,
                                          Args &&... args) const {
    switch (material.type) {
      case BRDF::Phong:
        return phong[material.id].SampleBrdf(std::forward<Args>(args)...);
      case BRDF::GGX:
        return ggx[material.id].SampleBrdf(std::forward<Args>(args)...);
    }
  }

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