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
  struct Bounce {
    const BSDF* bsdf;
    const glm::vec3 w_o;

    glm::vec3 Value(const glm::vec3& w_i) const { return bsdf->Brdf(w_i, w_o); }

    glm::vec3 Sample() const { return bsdf->Sample(w_o); }

    float Pdf(const glm::vec3& w_i) const { return bsdf->Pdf(w_o, w_i); }
  };

  glm::vec3 ShadePixel(const glm::vec2& pixel) const override;

  glm::vec3 LightIndirect(
      const glm::vec3& x, const glm::vec3& n, const Bounce& bounce,
      std::conditional_t<russian_roulette, const glm::vec3&, int>
          throughput_or_depth) const;

  glm::vec3 LightDirect(const glm::vec3& x, const glm::vec3& n,
                        const Bounce& bounce) const;

  template <bool T = mis>
  std::enable_if_t<T, float> PdfLight(const glm::vec3& x, const glm::vec3& n,
                                      const glm::vec3& w_i) const;

  template <bool brdf, bool T = mis>
  std::enable_if_t<T, glm::vec3> MISSample(const glm::vec3& x,
                                           const glm::vec3& n,
                                           const glm::vec3& w_i,
                                           const Bounce& bounce) const;

  std::conditional_t<russian_roulette, std::monostate, int> max_depth_;
  int num_pixel_sample_;
};

template <bool mis>
using PathTracerNEE = PathTracer<false, mis>;
template <bool mis>
using PathTracerRR = PathTracer<true, mis>;

#include "path_tracer.inc"
