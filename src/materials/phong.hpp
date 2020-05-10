#pragma once

#include <glm/gtx/component_wise.hpp>

#include "../samplers/random.hpp"
#include "../samplers/sampler.hpp"
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
           k_s_ * (s_ + 2) / 2.f * pow(max(0.f, dot(r, w_i)), s_) * one_over_pi;
  }

  std::tuple<glm::vec3, float> SampleBrdf(const glm::vec3 &n,
                                          const glm::vec3 &w_o) const override {
    using namespace glm;

    const auto t = compMax(k_s_) / compMax(k_d_ + k_s_);
    const auto r = reflect(-w_o, n);
    const auto w_i = Random() <= t ? SampleSpecular(r, s_) : SampleDiffuse(n);
    const auto pdf = (1 - t) * max(0.f, dot(n, w_i)) * one_over_pi +
                     t * (s_ + 1) * Material::one_over_two_pi *
                         pow(max(0.f, dot(r, w_i)), s_);
    return std::make_tuple(w_i, pdf);
  }

  friend bool operator==(const Phong &x, const Phong &y) {
    return x.k_d_ == y.k_d_ && x.k_s_ == y.k_s_ && x.s_ == y.s_;
  }

  friend bool operator!=(const Phong &x, const Phong &y) { return !(x == y); }

 private:
  glm::vec3 SampleSpecular(const glm::vec3 &r, const float s) const {
    using namespace glm;
    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(pow(xi_1, 1 / (s + 1))),
               phi = Material::two_pi * xi_2;
    return ConvertSpherical(theta, phi, r);
  }

  float s_;
};
