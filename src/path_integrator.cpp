#include "raytracer/integrators/path_integrator.hpp"

#include <glm/gtc/constants.hpp>

glm::vec3 PathIntegrator::Sample(const Ray &ray, int depth) const {
  using namespace glm;

  if (depth <= 0) {
    return vec3(0.f);
  }

  return scene_.Trace(
      ray, [&](const LightEmission &emission) { return emission.L_e; },
      [&](const RayHit &hit) {
        const auto brdf = hit.material->GetBRDF(hit);
        const auto x = ray(hit.t), n = normalize(hit.normal);
        const auto w_i = normalize(sphere_sampler_.Sample(n)),
                   w_o = -normalize(ray.direction);
        const auto L_r = Sample({x + w_i * SHADOW_EPSILON, w_i}, depth - 1);
        return 2 * glm::pi<float>() * L_r * brdf(w_i, w_o) * dot(n, w_i);
      });
}

glm::vec3 PathIntegrator::SampleNEE(const Ray &ray) const {
  using namespace glm;

  return scene_.Trace(
      ray, [&](const LightEmission &emission) { return emission.L_e; },
      [&](const RayHit &hit) {
        const auto brdf = hit.material->GetBRDF(hit);
        const auto x = ray(hit.t), n = hit.normal;
        const auto w_o = -ray.direction;
        return LightDirect(x, n, w_o, brdf) +
               LightIndirect(x, n, w_o, brdf, max_depth_ - 1);
      });
}

glm::vec3 PathIntegrator::LightIndirect(const glm::vec3 &x, const glm::vec3 &n,
                                        const glm::vec3 &w_o, const BRDF &brdf,
                                        int depth) const {
  using namespace glm;

  if (depth <= 0) {
    return vec3(0.f);
  }

  const auto w_i = normalize(sphere_sampler_.Sample(n));
  const auto ray_i = Ray{x + w_i * SHADOW_EPSILON, w_i};

  return scene_.Trace(
      ray_i, [&](const LightEmission &emission) { return vec3(0.f); },
      [&](const RayHit &hit) {
        const auto brdf_i = hit.material->GetBRDF(hit);
        const auto x_i = ray_i(hit.t), n_i = hit.normal;
        const auto L = LightDirect(x_i, n_i, -w_i, brdf_i) +
                       LightIndirect(x_i, n_i, -w_i, brdf_i, depth - 1);
        return 2 * glm::pi<float>() * L * brdf(w_i, w_o) * dot(n, w_i);
      });
}
