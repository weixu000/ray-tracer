#include "raytracer/integrators/path_integrator.hpp"

#include <glm/gtc/constants.hpp>

glm::vec3 PathIntegrator::ShadePixel(const glm::vec2 &pixel) const {
  auto radiance = Sample(camera_.GenerateEyeRay(pixel + .5f),
                         max_depth_);  // first sample is in the center
  for (int i = 0; i < num_sample_ - 1; ++i) {
    radiance += Sample(camera_.GenerateEyeRay(pixel + pixel_sampler_.Sample()),
                       max_depth_);
  }
  return radiance / float(num_sample_);
}

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
    const auto w_o = -normalize(ray.direction),
               w_i = normalize(sphere_sampler_.Sample(n));
    const auto L_r = Sample({x + w_i * SHADOW_EPSILON, w_i}, depth - 1);
    radiance = 2 * glm::pi<float>() * L_r * hit->brdf(w_i, w_o) * dot(n, w_i);
  }
  return radiance;
}