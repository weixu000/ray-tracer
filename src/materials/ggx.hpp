#pragma once

#include <glm/gtx/component_wise.hpp>

#include "material.hpp"

class GGX : public Material {
 public:
  template <typename... Args>
  GGX(float alpha, Args... args)
      : Material(std::forward<Args>(args)...), alpha_(alpha) {}

  friend bool operator==(const GGX &x, const GGX &y) {
    return x.k_d_ == y.k_d_ && x.k_s_ == y.k_s_ && x.alpha_ == y.alpha_;
  }

  friend bool operator!=(const GGX &x, const GGX &y) { return !(x == y); }

 private:
  glm::vec3 BrdfSpecular(const glm::vec3 &n, const glm::vec3 &w_i,
                         const glm::vec3 &w_o) const override {
    using namespace glm;

    const auto w_i_n = dot(w_i, n), w_o_n = dot(w_o, n);
    if (w_i_n < 0 || w_o_n < 0) {
      return vec3{0.f};
    }

    const auto h = normalize(w_i + w_o);
    const auto ggx = F(dot(w_i, h)) * G(w_i_n) * G(w_o_n) * D(dot(h, n)) /
                     (4 * w_i_n * w_o_n);
    return ggx;
  }

  float PdfSpecular(const glm::vec3 &n, const glm::vec3 &w_i,
                    const glm::vec3 &w_o) const override {
    using namespace glm;
    const auto h = normalize(w_i + w_o);
    const auto h_n = abs(dot(h, n));
    return D(h_n) * h_n / (4 * abs(dot(h, w_o)));
  }

  glm::vec3 SampleSpecular(const glm::vec3 &n,
                           const glm::vec3 &w_i) const override {
    using namespace glm;
    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = atan(alpha_ * sqrt(xi_1 / (1 - xi_1))),
               phi = TWO_PI * xi_2;
    const auto h = ConvertSpherical(theta, phi, n);
    return reflect(-w_i, h);
  }

  float ProbSpecular() const override {
    using namespace glm;
    return max(.25f, compMax(k_s_) / compMax(k_d_ + k_s_));
  }

  glm::vec3 F(float w_i_h) const {
    using namespace glm;
    return k_s_ + (1.f - k_s_) * pow(1.f - w_i_h, 5.f);
  }

  float D(float h_n) const {
    return pow(alpha_, 2) /
           (PI * pow(h_n, 4) * pow(pow(alpha_, 2) + 1 / pow(h_n, 2) - 1, 2));
  }

  float G(float v_n) const {
    return 2 / (1 + sqrt(1 + pow(alpha_, 2) * (1 / pow(v_n, 2) - 1)));
  }

  float alpha_;
};