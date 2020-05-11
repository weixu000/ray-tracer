#pragma once

#include <type_traits>
#include <variant>

#include "../samplers/sampler.hpp"
#include "integrator.hpp"

enum class Sampling { Hemisphere, Cosine, BRDF };

template <Sampling sampling>
std::tuple<glm::vec3, float> ImportanceSample(const MaterialRef& material,
                                              const glm::vec3& n,
                                              const glm::vec3& w_o,
                                              const Scene& scene) {
  switch (sampling) {
    case Sampling::Hemisphere: {
      return SampleHemisphere(n);
    }
    case Sampling::Cosine: {
      return SampleConsine(n);
    }
    case Sampling::BRDF:
      return scene.SampleBrdf(material, n, w_o);
  }
}

template <bool russian_roulette, Sampling sampling>
class PathIntegrator : public Integrator {
 public:
  template <bool T = russian_roulette, typename... Args>
  PathIntegrator(std::enable_if_t<T, int> num_pixel_sample, Args&&... args)
      : Integrator(std::forward<Args>(args)...),
        num_pixel_sample_(num_pixel_sample) {}

  template <bool T = !russian_roulette, typename... Args>
  PathIntegrator(std::enable_if_t<T, int> max_depth, int num_pixel_sample,
                 Args&&... args)
      : Integrator(std::forward<Args>(args)...),
        num_pixel_sample_(num_pixel_sample),
        max_depth_(max_depth) {}

 private:
  glm::vec3 ShadePixel(const glm::vec2& pixel) const override {
    auto radiance = glm::vec3(0.f);
    for (int i = 0; i < num_pixel_sample_; ++i) {
      radiance += Sample(camera_.GenerateEyeRay(pixel + SampleSquare()));
    }
    return radiance / float(num_pixel_sample_);
  }

  glm::vec3 Sample(const Ray& ray) const;

  glm::vec3 LightIndirect(
      const glm::vec3& x, const glm::vec3& n, const glm::vec3& w_o,
      const MaterialRef& material,
      std::conditional_t<russian_roulette, const glm::vec3&, int>
          throughput_or_depth) const;

  std::tuple<glm::vec3, float> SampleBrdf(const MaterialRef& material,
                                          const glm::vec3& n,
                                          const glm::vec3& w_o) const {
    return ImportanceSample<sampling>(material, n, w_o, scene_);
  }

  glm::vec3 LightDirect(const glm::vec3& x, const glm::vec3& n,
                        const glm::vec3& w_o,
                        const MaterialRef& material) const;

  std::conditional_t<russian_roulette, std::monostate, int> max_depth_;
  int num_pixel_sample_;
};

template <Sampling sampling>
using IntegratorNEE = PathIntegrator<false, sampling>;
template <Sampling sampling>
using IntegratorRR = PathIntegrator<true, sampling>;

#include "path_integrator.inc"
