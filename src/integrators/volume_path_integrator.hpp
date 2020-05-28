#pragma once

#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>

#include "../materials/bssrdf.hpp"
#include "integrator.hpp"

class VolumePathIntegrator : public Integrator {
 public:
  template <typename... Args>
  VolumePathIntegrator(int num_pixel_sample, Args&&... args)
      : Integrator(std::forward<Args>(args)...),
        num_pixel_sample_(num_pixel_sample) {}

 private:
  glm::vec3 ShadePixel(const glm::vec2& pixel) const override {
    using namespace glm;

    auto radiance = vec3(0.f);
    for (int i = 0; i < num_pixel_sample_; ++i) {
      const auto ray = camera_.GenerateEyeRay(pixel + vec2{Random(), Random()});
      radiance += Trace(
          ray, [&](const Emission& emission) { return emission.L_e; },
          [&](const RayHit& hit) {
            const auto x = ray(hit.t), n = hit.n;
            const auto w_o = -ray.direction;
            return LightDirect(x, n, w_o);
          });
    }
    return radiance / float(num_pixel_sample_);
  }

  glm::vec3 LightDirect(const glm::vec3& x_o, const glm::vec3& n_o,
                        const glm::vec3& w_o) const {
    using namespace glm;

    if (const auto sample = bssrdf_.SamplePi(x_o, n_o, [this](auto&&... args) {
          kernel_.TraceShapesAll(std::forward<decltype(args)>(args)...);
        })) {
      const auto [hit, pdf_x_i] = *sample;
      const auto x_i = hit.p;
      const auto n_i = hit.n;

      auto radiance = vec3{0.f};

      for (const auto& light : lights_) {
        const auto sample = light->Sample();
        const auto w_i = normalize(sample - x_i);
        const auto ray_i =
            Ray{x_i + sign(dot(w_i, n_i)) * n_i * SHADOW_EPSILON, w_i};
        if (const auto light_hit = TraceLights(ray_i);
            light_hit && light_hit->light == light.get()) {
          if (const auto shadow_hit = kernel_.TraceShapes(ray_i);
              !shadow_hit || shadow_hit->t > light_hit->t) {
            const auto n_l = light_hit->n;
            const auto G = abs(dot(n_i, w_i)) * max(0.f, dot(w_i, n_l)) /
                           length2(sample - x_i);
            radiance += light_hit->L_e * light_hit->jacobian *
                        bssrdf_.Value(x_i, n_i, w_i, x_o, n_o, w_o) * G;
          }
        }
      }

      return radiance / pdf_x_i;
    } else
      return vec3{0.f};
  }

  int num_pixel_sample_;
  BSSRDF bssrdf_;
};
