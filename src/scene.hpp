#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "lights/light.hpp"
#include "materials/ggx.hpp"
#include "materials/phong.hpp"
#include "shapes/bvh.hpp"

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
  glm::vec3 Sample(const MaterialRef &material, Args &&... args) const {
    switch (material.type) {
      case BRDF::Phong:
        return phong[material.id].Sample(std::forward<Args>(args)...);
      case BRDF::GGX:
        return ggx[material.id].Sample(std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  float Pdf(const MaterialRef &material, Args &&... args) const {
    switch (material.type) {
      case BRDF::Phong:
        return phong[material.id].Pdf(std::forward<Args>(args)...);
      case BRDF::GGX:
        return ggx[material.id].Pdf(std::forward<Args>(args)...);
    }
  }

  std::optional<RayHit> TraceShapes(const Ray &ray) const {
    return group.Hit(ray);
  }

  std::optional<LightEmission> TraceLights(const Ray &ray) const {
    std::optional<LightEmission> ret;
    for (const auto &light : lights) {
      if (const auto hit = light->Hit(ray); hit && (!ret || hit->t < ret->t)) {
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
    if (shape_hit && (!light_hit || shape_hit->t < light_hit->t)) {
      return if_shape(*shape_hit);
    } else if (light_hit) {
      return if_light(*light_hit);
    } else {
      return default_val;
    }
  }
};