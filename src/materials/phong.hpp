#pragma once

#include <glm/gtx/component_wise.hpp>

#include "../samplers/sampling.hpp"
#include "material.hpp"

class Phong : public Material {
 public:
  Phong(const glm::vec3 &k_s, float s) : s_(s), k_s_(k_s) {}

  friend bool operator==(const Phong &x, const Phong &y) {
    return x.k_s_ == y.k_s_ && x.s_ == y.s_;
  }

  friend bool operator!=(const Phong &x, const Phong &y) { return !(x == y); }

  glm::vec3 Brdf(const glm::vec3 &n, const glm::vec3 &w_i,
                 const glm::vec3 &w_o) const override {
    using namespace glm;
    const auto r = reflect(-w_i, n);
    return k_s_ * (s_ + 2) / 2.f * pow(max(0.f, dot(r, w_o)), s_) * ONE_OVER_PI;
  }

  glm::vec3 Sample(const glm::vec3 &n, const glm::vec3 &w_i) const override {
    using namespace glm;
    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(pow(xi_1, 1 / (s_ + 1))), phi = TWO_PI * xi_2;
    const auto r = reflect(-w_i, n);
    return ConvertSpherical(theta, phi, r);
  }

  float Pdf(const glm::vec3 &n, const glm::vec3 &w_i,
            const glm::vec3 &w_o) const override {
    using namespace glm;
    const auto r = reflect(-w_i, n);
    return (s_ + 1) * TWO_OVER_PI * pow(max(0.f, dot(r, w_o)), s_);
  }

  float Power(const glm::vec3 &n, const glm::vec3 &w_i) const override {
    return glm::compAdd(k_s_);
  }

 private:
  float s_;
  glm::vec3 k_s_;
};
