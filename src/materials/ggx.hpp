#pragma once

#include <glm/gtx/component_wise.hpp>

#include "../samplers/sampling.hpp"
#include "composed_bsdf.hpp"
#include "lambertian.hpp"

template <bool transmission>
class GGXBSDF {
 public:
  GGXBSDF(const glm::vec3 &n, const glm::vec3 &k_s, float alpha)
      : n{n}, k_s{k_s}, alpha{alpha} {
    const auto x = glm::sqrt(k_s);
    ior = (1.f + x) / (1.f - x);
  }

  glm::vec3 Value(const glm::vec3 &w_i, const glm::vec3 &w_o) const;

  glm::vec3 Sample(const glm::vec3 &w_i) const;

  float Pdf(const glm::vec3 &w_i, const glm::vec3 &w_o) const;

  float Weight(const glm::vec3 &w_i) const;

 private:
  glm::vec3 n;
  glm::vec3 k_s;
  float alpha;
  glm::vec3 ior;
};

extern template class GGXBSDF<true>;
extern template class GGXBSDF<false>;

class GGXReflection {
 public:
  GGXReflection(const glm::vec3 &k_d, const glm::vec3 &k_s, float alpha)
      : k_d_(k_d), alpha_(alpha), k_s_(k_s) {}

  GGXReflection(const glm::vec3 &k_d, float n, float alpha)
      : k_d_(k_d), alpha_(alpha), k_s_(glm::pow((n - 1) / (n + 1), 2.f)) {}

  auto GetBxDF(const glm::vec3 &n) const {
    ComposedBSDF<LambertianBRDF, GGXBSDF<false>> bsdf{
        LambertianBRDF{n, k_d_}, GGXBSDF<false>{n, k_s_, alpha_}};
    return bsdf;
  }

 private:
  float alpha_;
  glm::vec3 k_s_;
  glm::vec3 k_d_;
};

class GGXRefraction {
 public:
  GGXRefraction(glm::vec3 k_s, float alpha) : alpha_(alpha), k_s_(k_s) {}

  auto GetBxDF(const glm::vec3 &n) const {
    return GGXBSDF<true>{n, k_s_, alpha_};
  }

 private:
  float alpha_;
  glm::vec3 k_s_;
};