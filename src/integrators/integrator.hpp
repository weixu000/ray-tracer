#pragma once

#include <glm/glm.hpp>

#include "../camera.hpp"
#include "../image.hpp"
#include "../kernel.hpp"
#include "../scene.hpp"

/**
 * Use camera to render the scene
 */
class Integrator {
 public:
  Integrator(Scene scene, Camera camera, float gamma);

  virtual ~Integrator() = default;

  Image Render() const;

 protected:
  std::optional<Emission> TraceLights(const Ray &ray) const {
    std::optional<Emission> ret;
    for (const auto &light : lights_) {
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
    const auto shape_hit = kernel_.TraceShapes(ray);
    if (shape_hit && (!light_hit || shape_hit->t < light_hit->t)) {
      return if_shape(*shape_hit);
    } else if (light_hit) {
      return if_light(*light_hit);
    } else {
      return default_val;
    }
  }

  Camera camera_;
  std::vector<std::unique_ptr<const Light>> lights_;
  std::vector<Material> materials_;

  Kernel kernel_;

 private:
  virtual glm::vec3 ShadePixel(const glm::vec2 &pixel) const = 0;

  float gamma_;
};
