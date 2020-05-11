#pragma once

#include <type_traits>
#include <variant>

#include "../samplers/sampler.hpp"
#include "integrator.hpp"

enum class Sampling { Hemisphere, Cosine, BRDF };

template <bool russian_roulette, Sampling sampling, bool mis>
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

  glm::vec3 SampleBrdf(const MaterialRef& material, const glm::vec3& n,
                       const glm::vec3& w_o) const;
  float PdfBrdf(const MaterialRef& material, const glm::vec3& n,
                const glm::vec3& w_i, const glm::vec3& w_o) const;

  float PdfLight(const glm::vec3& x, const glm::vec3& w_i) const;

  glm::vec3 LightDirect(const glm::vec3& x, const glm::vec3& n,
                        const glm::vec3& w_o, const MaterialRef& mat) const;

  template <bool brdf, bool T = mis>
  std::enable_if_t<T, glm::vec3> MISSample(const glm::vec3& x,
                                           const glm::vec3& n,
                                           const glm::vec3& w_i,
                                           const glm::vec3& w_o,
                                           const MaterialRef& mat) const;

  std::conditional_t<russian_roulette, std::monostate, int> max_depth_;
  int num_pixel_sample_;
};

template <Sampling sampling, bool mis>
using IntegratorNEE = PathIntegrator<false, sampling, mis>;
template <Sampling sampling, bool mis>
using IntegratorRR = PathIntegrator<true, sampling, mis>;

#include "path_integrator.inc"
