#pragma once

#include "integrator.hpp"

class PathTracer : public Integrator {
 public:
  template <typename... Args>
  PathTracer(bool russian_roulette, bool mis, int max_depth,
             int num_pixel_sample, Args&&... args)
      : Integrator(std::forward<Args>(args)...),
        num_pixel_sample_(num_pixel_sample),
        max_depth_(max_depth),
        russian_roulette_(russian_roulette),
        mis_(mis) {}

 private:
  glm::vec3 ShadePixel(const glm::vec2& pixel) const override;

  template <typename Bounce>
  glm::vec3 LightIndirect(const glm::vec3& x, const glm::vec3& n,
                          const Bounce& bounce, const glm::vec3& throughput,
                          int depth) const;

  template <typename Bounce>
  glm::vec3 LightDirect(const glm::vec3& x, const glm::vec3& n,
                        const Bounce& bounce) const;

  float PdfLight(const glm::vec3& x, const glm::vec3& n,
                 const glm::vec3& w_i) const;

  template <bool brdf, typename Bounce>
  glm::vec3 MISSample(const glm::vec3& x, const glm::vec3& n,
                      const glm::vec3& w_i, const Bounce& bounce) const;

  int max_depth_;
  int num_pixel_sample_;

  bool russian_roulette_;
  bool mis_;
};
