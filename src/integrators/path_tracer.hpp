#pragma once

#include <type_traits>
#include <variant>

#include "../samplers/sampler.hpp"
#include "integrator.hpp"

template <bool russian_roulette, bool mis>
class PathTracer : public Integrator {
 public:
  template <bool T = russian_roulette, typename... Args>
  PathTracer(std::enable_if_t<T, int> num_pixel_sample, Args&&... args)
      : Integrator(std::forward<Args>(args)...),
        num_pixel_sample_(num_pixel_sample) {}

  template <bool T = !russian_roulette, typename... Args>
  PathTracer(std::enable_if_t<T, int> max_depth, int num_pixel_sample,
             Args&&... args)
      : Integrator(std::forward<Args>(args)...),
        num_pixel_sample_(num_pixel_sample),
        max_depth_(max_depth) {}

 private:
  glm::vec3 ShadePixel(const glm::vec2& pixel) const override;

  glm::vec3 LightIndirect(
      const glm::vec3& x, const glm::vec3& n, const glm::vec3& w_o,
      const Material* mat,
      std::conditional_t<russian_roulette, const glm::vec3&, int>
          throughput_or_depth) const;

  glm::vec3 LightDirect(const glm::vec3& x, const glm::vec3& n,
                        const glm::vec3& w_o, const Material* mat) const;

  template <bool T = mis>
  std::enable_if_t<T, float> PdfLight(const glm::vec3& x, const glm::vec3& n,
                                      const glm::vec3& w_i) const;

  template <bool brdf, bool T = mis>
  std::enable_if_t<T, glm::vec3> MISSample(const glm::vec3& x,
                                           const glm::vec3& n,
                                           const glm::vec3& w_i,
                                           const glm::vec3& w_o,
                                           const Material* mat) const;

  std::conditional_t<russian_roulette, std::monostate, int> max_depth_;
  int num_pixel_sample_;
};

template <bool mis>
using PathTracerNEE = PathTracer<false, mis>;
template <bool mis>
using PathTracerRR = PathTracer<true, mis>;

#include "path_tracer.inc"
