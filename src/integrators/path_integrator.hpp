#pragma once

#include <type_traits>
#include <variant>

#include "../samplers/independent_multisampler.hpp"
#include "direct_integrator.hpp"

template <typename Sampler>
class PathIntegratorSimple : public Integrator {
 public:
  PathIntegratorSimple(const Scene& scene, const Camera& camera, int num_sample,
                       int max_depth)
      : Integrator(scene, camera),
        max_depth_(max_depth),
        pixel_sampler_(num_sample) {}

 private:
  glm::vec3 ShadePixel(const glm::vec2& pixel) const override {
    auto radiance = glm::vec3(0.f);
    for (const auto& sample : pixel_sampler_.Sample()) {
      radiance += Sample(camera_.GenerateEyeRay(pixel + sample), max_depth_);
    }
    return radiance / float(pixel_sampler_.Count());
  }

  glm::vec3 Sample(const Ray& ray, int depth) const;

  int max_depth_;

  Sampler sphere_sampler_;
  SquareMultiampler pixel_sampler_;
};

template <bool russian_roulette, typename LightSampler, typename Sampler>
class PathIntegrator : public DirectIntegrator<LightSampler> {
 public:
  using Base = DirectIntegrator<LightSampler>;

  PathIntegrator(const Scene& scene, const Camera& camera, int num_pixel_sample,
                 int num_light_sample)
      : Base(scene, camera, num_light_sample),
        pixel_sampler_(num_pixel_sample) {}

  template <bool T = !russian_roulette>
  PathIntegrator(const Scene& scene, const Camera& camera, int num_pixel_sample,
                 int num_light_sample, std::enable_if_t<T, int> max_depth)
      : PathIntegrator(scene, camera, num_pixel_sample, num_light_sample) {
    max_depth_ = max_depth;
  }

 private:
  glm::vec3 ShadePixel(const glm::vec2& pixel) const override {
    auto radiance = glm::vec3(0.f);
    for (const auto& sample : pixel_sampler_.Sample()) {
      radiance += Sample(Base::camera_.GenerateEyeRay(pixel + sample));
    }
    return radiance / float(pixel_sampler_.Count());
  }

  glm::vec3 Sample(const Ray& ray) const;

  glm::vec3 LightIndirect(
      const glm::vec3& x, const glm::vec3& n, const glm::vec3& w_o,
      const BRDF& brdf,
      std::conditional_t<russian_roulette, const glm::vec3&, int>
          throughput_or_depth) const;

  std::conditional_t<russian_roulette, std::monostate, int> max_depth_;

  Sampler sphere_sampler_;
  SquareMultiampler pixel_sampler_;
};

template <typename LightSampler, typename Sampler>
using IntegratorNEE = PathIntegrator<false, LightSampler, Sampler>;
template <typename LightSampler, typename Sampler>
using IntegratorRR = PathIntegrator<true, LightSampler, Sampler>;

#include "path_integrator.inc"
