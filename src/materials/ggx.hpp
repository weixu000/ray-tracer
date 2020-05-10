#pragma once

#include <glm/gtx/component_wise.hpp>

#include "../samplers/random.hpp"
#include "../samplers/sampler.hpp"
#include "material.hpp"

class GGX : public Material {
 public:
  template <typename... Args>
  GGX(float alpha, Args... args)
      : Material(std::forward<Args>(args)...), alpha_(alpha) {}

  glm::vec3 Brdf(const glm::vec3 &n, const glm::vec3 &w_i,
                 const glm::vec3 &w_o) const override {
    using namespace glm;

    const auto w_i_n = dot(w_i, n), w_o_n = dot(w_o, n);
    if (w_i_n < 0 || w_o_n < 0) {
      return vec3{0.f};
    }

    const auto h = normalize(w_i + w_o);
    const auto F = k_s_ + (1.f - k_s_) * float(pow(1 - dot(w_i, h), 5));
    const auto ggx =
        F * G(w_i_n) * G(w_o_n) * D(dot(h, n)) / (4 * w_i_n * w_o_n);
    return DiffuseBrdf() + ggx;
  }

  std::tuple<glm::vec3, float> SampleBrdf(const glm::vec3 &n,
                                          const glm::vec3 &w_o) const override {
    using namespace glm;

    const auto t = max(.25f, compMax(k_s_) / compMax(k_d_ + k_s_));
    const auto w_i = Random() <= t ? SampleSpecular(n, w_o) : SampleDiffuse(n);
    const auto h = normalize(w_i + w_o);
    const auto h_n = dot(h, n);
    const auto pdf = (1 - t) * max(0.f, dot(n, w_i)) * one_over_pi +
                     t * D(h_n) * h_n / (4 * dot(h, w_i));
    return std::make_tuple(w_i, pdf);
  }

  friend bool operator==(const GGX &x, const GGX &y) {
    return x.k_d_ == y.k_d_ && x.k_s_ == y.k_s_ && x.alpha_ == y.alpha_;
  }

  friend bool operator!=(const GGX &x, const GGX &y) { return !(x == y); }

 private:
  float D(float h_n) const {
    return pow(alpha_, 2) /
           (pi * pow(h_n, 4) * pow(pow(alpha_, 2) + 1 / pow(h_n, 2) - 1, 2));
  }

  float G(float v_n) const {
    return 2 / (1 + sqrt(1 + pow(alpha_, 2) * (1 / pow(v_n, 2) - 1)));
  }

  glm::vec3 SampleSpecular(const glm::vec3 &n, const glm::vec3 &w_o) const {
    using namespace glm;
    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = atan(alpha_ * sqrt(xi_1 / (1 - xi_1))),
               phi = two_pi * xi_2;
    const auto h = ConvertSpherical(theta, phi, n);
    return reflect(-w_o, h);
  }

  float alpha_;
};