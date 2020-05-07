#pragma once

#include <memory>

#include "integrator.hpp"

template <typename Sampler>
class DirectIntegrator : public Integrator {
 public:
  DirectIntegrator(const Scene& scene, const Camera& camera, int light_sample)
      : Integrator(scene, camera), sampler_(light_sample) {}

 protected:
  glm::vec3 LightDirect(const glm::vec3& x, const glm::vec3& n,
                        const glm::vec3& w_o, const BRDF& brdf) const;

 private:
  glm::vec3 ShadePixel(const glm::vec2& pixel) const override {
    using namespace glm;

    const auto ray = camera_.GenerateEyeRay(pixel + .5f);

    return scene_.Trace(
        ray, [&](const LightEmission& emission) { return emission.L_e; },
        [&](const RayHit& hit) {
          const auto brdf = hit.material->GetBRDF(hit);
          const auto x = ray(hit.t), n = hit.normal, w_o = -ray.direction;
          return LightDirect(x, n, w_o, brdf);
        });
  }

  Sampler sampler_;
};

#include "direct_integrator.inc"