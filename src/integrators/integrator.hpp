#pragma once

#include <glm/glm.hpp>

#include "../camera.hpp"
#include "../image.hpp"
#include "../scene.hpp"
#include "../shapes/bvh.hpp"

/**
 * Use camera to render the scene
 */
class Integrator {
 public:
  Integrator(Scene scene, Camera camera, float gamma)
      : scene_(std::move(scene)),
        camera_(std::move(camera)),
        gamma_(gamma),
        bvh_(std::move(scene_.shapes)) {}

  virtual ~Integrator() = default;

  Image Render() const;

 protected:
  std::optional<RayHit> TraceShapes(const Ray &ray) const {
    return bvh_.Hit(ray);
  }

  std::optional<LightEmission> TraceLights(const Ray &ray) const {
    std::optional<LightEmission> ret;
    for (const auto &light : scene_.lights) {
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

  Scene scene_;
  Camera camera_;

 private:
  virtual glm::vec3 ShadePixel(const glm::vec2 &pixel) const = 0;

  float gamma_;
  BVH bvh_;
};
