#include "raytracer/integrators/path_integrator.hpp"

#include <glm/gtc/constants.hpp>

glm::vec3 PathIntegrator::Sample(const Ray &ray, int depth) const {
  using namespace glm;

  if (depth <= 0) {
    return vec3(0.f);
  }

  auto radiance = vec3(0.f);
  auto t = FLT_MAX;
  if (const auto hit = scene_.TraceLight(ray)) {
    radiance = hit->L_e;
    t = hit->distance;
  }

  if (const auto hit = scene_.Trace(ray); hit && hit->t < t) {
    const auto x = ray(hit->t), n = normalize(hit->normal);
    const auto w_i = normalize(sphere_sampler_.Sample(n)),
               w_o = -normalize(ray.direction);
    const auto L_r = Sample({x + w_i * SHADOW_EPSILON, w_i}, depth - 1);
    radiance = 2 * glm::pi<float>() * L_r * hit->brdf(w_i, w_o) * dot(n, w_i);
  }
  return radiance;
}

glm::vec3 PathIntegrator::SampleNEE(const Ray &ray) const {
  using namespace glm;

  auto radiance = vec3(0.f);
  auto t = FLT_MAX;
  if (const auto hit = scene_.TraceLight(ray)) {
    radiance = hit->L_e;
    t = hit->distance;
  }

  if (const auto hit = scene_.Trace(ray); hit && hit->t < t) {
    const auto &brdf = hit->brdf;
    const auto x = ray(hit->t), n = normalize(hit->normal);
    const auto w_o = -normalize(ray.direction);
    radiance = LightDirect(x, n, w_o, brdf) +
               LightIndirect(x, n, w_o, brdf, max_depth_ - 1);
  }
  return radiance;
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

  auto t = FLT_MAX;
  if (const auto hit = scene_.TraceLight(ray_i)) {
    t = hit->distance;
  }

  if (const auto hit = scene_.Trace(ray_i); hit && hit->t < t) {
    const auto &brdf_i = hit->brdf;
    const auto x_i = ray_i(hit->t), n_i = normalize(hit->normal);
    const auto L = LightDirect(x_i, n_i, -w_i, brdf_i) +
                   LightIndirect(x_i, n_i, -w_i, brdf_i, depth - 1);
    return 2 * glm::pi<float>() * L * brdf(w_i, w_o) * dot(n, w_i);
  } else {
    return vec3(0.f);
  }
}
