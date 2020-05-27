#pragma once

#include <glm/gtx/component_wise.hpp>

#include "../samplers/sampling.hpp"
#include "composed_bsdf.hpp"
#include "lambertian.hpp"

struct PhongBRDF : public BSDF {
  glm::vec3 Value(const glm::vec3 &w_i, const glm::vec3 &w_o) const override {
    using namespace glm;
    const auto r = reflect(-w_i, n);
    return k_s * (s + 2) / 2.f * pow(max(0.f, dot(r, w_o)), s) * ONE_OVER_PI;
  }

  glm::vec3 Sample(const glm::vec3 &w_i) const override {
    using namespace glm;
    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(pow(xi_1, 1 / (s + 1))), phi = TWO_PI * xi_2;
    const auto r = reflect(-w_i, n);
    return ConvertSpherical(theta, phi, r);
  }

  float Pdf(const glm::vec3 &w_i, const glm::vec3 &w_o) const override {
    using namespace glm;
    const auto r = reflect(-w_i, n);
    return (s + 1) * ONE_OVER_2PI * pow(max(0.f, dot(r, w_o)), s);
  }

  float Weight(const glm::vec3 &w_i) const override {
    return glm::compAdd(k_s);
  }

  glm::vec3 n;
  float s;
  glm::vec3 k_s;
};

class Phong {
 public:
  Phong(const glm::vec3 &k_d, const glm::vec3 &k_s, float s)
      : k_d_(k_d), s_(s), k_s_(k_s) {}

  auto GetBSDF(const glm::vec3 &n) const {
    ComposedBSDF<LambertianBRDF, PhongBRDF> bsdf;

    std::get<0>(bsdf.bsdfs).n = n;
    std::get<0>(bsdf.bsdfs).k_d = k_d_;

    std::get<1>(bsdf.bsdfs).n = n;
    std::get<1>(bsdf.bsdfs).s = s_;
    std::get<1>(bsdf.bsdfs).k_s = k_s_;

    return bsdf;
  }

 private:
  float s_;
  glm::vec3 k_s_;
  glm::vec3 k_d_;
};
