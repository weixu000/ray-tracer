#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <optional>
#include <tuple>

#include "../ray.hpp"
#include "../samplers/sampler.hpp"
#include "../samplers/sampling.hpp"

struct NormalizedDiffusionProfile {
  NormalizedDiffusionProfile(float d) : d(d), r_max(10 * d) {}

  float R(float r) const;

  glm::vec2 SampleR() const;

  float PdfR(float r) const { return R(r); }

  float d, r_max;
};

struct BSSRDF {
  BSSRDF(float A = .8f, float n = 1.5f)
      : A(A),
        R_0(glm::pow((n - 1) / (n + 1), 2)),
        profile{1 / (1.85f - A + 7 * glm::pow(glm::abs(A - .8f), 3.f))} {}

  glm::vec3 Value(const glm::vec3 &p_i, const glm::vec3 &n_i,
                  const glm::vec3 &w_i, const glm::vec3 &p_o,
                  const glm::vec3 &n_o, const glm::vec3 &w_o) const;

  using ProbeFunc = std::function<void(std::vector<RayHit> &out, const Ray &ray,
                                       float tnear, float tfar)>;

  std::optional<std::tuple<RayHit, float>> SamplePi(
      const glm::vec3 &p_o, const glm::vec3 &n_o,
      const ProbeFunc &hit_func) const;

  glm::vec3 SampleWi(const glm::vec3 &n_i) const { return sampler.Sample(n_i); }

  float PdfWi(const glm::vec3 &n_i, const glm::vec3 &w_i) const {
    return sampler.Pdf(n_i, w_i);
  }

 private:
  float F(float v_n) const;

  float A;
  float R_0;
  NormalizedDiffusionProfile profile;
  CosineSampler sampler;
};
