#pragma once

#include <glm/gtx/component_wise.hpp>

#include "material.hpp"

class Phong : public Material {
 public:
  template <typename... Args>
  Phong(float s, Args... args) : Material(std::forward<Args>(args)...), s_(s) {}

  friend bool operator==(const Phong &x, const Phong &y) {
    return x.k_d_ == y.k_d_ && x.k_s_ == y.k_s_ && x.s_ == y.s_;
  }

  friend bool operator!=(const Phong &x, const Phong &y) { return !(x == y); }

 private:
  glm::vec3 BrdfSpecular(const glm::vec3 &n, const glm::vec3 &w_i,
                         const glm::vec3 &w_o) const override {
    using namespace glm;
    const auto r = reflect(-w_o, n);
    return k_s_ * (s_ + 2) / 2.f * pow(max(0.f, dot(r, w_i)), s_) * ONE_OVER_PI;
  }

  glm::vec3 SampleSpecular(const glm::vec3 &n,
                           const glm::vec3 &w_o) const override {
    using namespace glm;
    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(pow(xi_1, 1 / (s_ + 1))), phi = TWO_PI * xi_2;
    const auto r = reflect(-w_o, n);
    return ConvertSpherical(theta, phi, r);
  }

  float PdfSpecular(const glm::vec3 &n, const glm::vec3 &w_i,
                    const glm::vec3 &w_o) const override {
    using namespace glm;
    const auto r = reflect(-w_o, n);
    return (s_ + 1) * TWO_OVER_PI * pow(max(0.f, dot(r, w_i)), s_);
  }

  float ProbSpecular() const override {
    using namespace glm;
    if (compMax(k_d_ + k_s_) == 0.f)
      return 0.5f;
    else
      return compMax(k_s_) / compMax(k_d_ + k_s_);
  }

  float s_;
};
