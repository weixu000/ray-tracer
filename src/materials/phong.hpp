#pragma once

#include <glm/gtx/component_wise.hpp>

#include "material.hpp"

class Phong : public Material {
 public:
  template <typename... Args>
  Phong(float s, Args... args) : Material(std::forward<Args>(args)...), s_(s) {}

  glm::vec3 Brdf(const glm::vec3 &n, const glm::vec3 &w_i,
                 const glm::vec3 &w_o) const override {
    using namespace glm;

    const auto r = reflect(-w_o, n);
    return DiffuseBrdf() +
           k_s_ * (s_ + 2) / 2.f * pow(max(0.f, dot(r, w_i)), s_) * ONE_OVER_PI;
  }

  glm::vec3 Sample(const glm::vec3 &n, const glm::vec3 &w_o) const override {
    using namespace glm;

    const auto r = reflect(-w_o, n);
    return Random() <= ProbSpecular() ? SampleSpecular(r, s_)
                                      : SampleDiffuse(n);
  }

  float Pdf(const glm::vec3 &n, const glm::vec3 &w_i,
            const glm::vec3 &w_o) const override {
    using namespace glm;
    const auto t = ProbSpecular();
    const auto r = reflect(-w_o, n);
    return (1 - t) * max(0.f, dot(n, w_i)) * ONE_OVER_PI +
           t * (s_ + 1) * TWO_OVER_PI * pow(max(0.f, dot(r, w_i)), s_);
  }

  friend bool operator==(const Phong &x, const Phong &y) {
    return x.k_d_ == y.k_d_ && x.k_s_ == y.k_s_ && x.s_ == y.s_;
  }

  friend bool operator!=(const Phong &x, const Phong &y) { return !(x == y); }

 private:
  float ProbSpecular() const {
    using namespace glm;
    return compMax(k_s_) / compMax(k_d_ + k_s_);
  }

  glm::vec3 SampleSpecular(const glm::vec3 &r, const float s) const {
    using namespace glm;
    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(pow(xi_1, 1 / (s + 1))), phi = TWO_PI * xi_2;
    return ConvertSpherical(theta, phi, r);
  }

  float s_;
};
