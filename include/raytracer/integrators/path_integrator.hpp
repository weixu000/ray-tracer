#pragma once

#include <random>

#include <raytracer/integrators/direct_integrator.hpp>
#include <raytracer/samplers/hemisphere_sampler.hpp>
#include <raytracer/samplers/square_sampler.hpp>

class PathIntegrator : public DirectIntegrator {
public:
  using DirectIntegrator::DirectIntegrator;

  int max_depth_ = 5;

  int num_sample_ = 1;

  bool next_event_ = false;

  bool russian_roulette_ = false;

private:
  glm::vec3 ShadePixel(const glm::vec2& pixel) const override {
    if (next_event_) {
      if (russian_roulette_) {
        return ShadePixel(pixel, [&](const Ray& ray) { return SampleRR(ray); });
      } else {
        return ShadePixel(pixel,
                          [&](const Ray& ray) { return SampleNEE(ray); });
      }
    } else {
      return ShadePixel(
          pixel, [&](const Ray& ray) { return Sample(ray, max_depth_); });
    }
  }

  template <typename T>
  glm::vec3 ShadePixel(const glm::vec2& pixel, T sample) const {
    auto radiance = glm::vec3(0.f);
    for (int i = 0; i < num_sample_; ++i) {
      radiance +=
          sample(camera_.GenerateEyeRay(pixel + pixel_sampler_.Sample()));
    }
    return radiance / float(num_sample_);
  }

  glm::vec3 Sample(const Ray& ray, int depth) const;

  glm::vec3 SampleNEE(const Ray& ray) const;

  glm::vec3 SampleRR(const Ray& ray) const;

  glm::vec3 LightIndirect(const glm::vec3& x, const glm::vec3& n,
                          const glm::vec3& w_o, const BRDF& brdf,
                          int depth) const;

  glm::vec3 LightIndirectRR(const glm::vec3& x, const glm::vec3& n,
                            const glm::vec3& w_o, const BRDF& brdf,
                            const glm::vec3& throughput) const;

  HemisphereSampler sphere_sampler_;
  SquareSampler pixel_sampler_;

  mutable std::default_random_engine gen_{std::random_device()()};
  mutable std::uniform_real_distribution<float> dist_;
};
