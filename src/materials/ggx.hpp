#pragma once

#include <glm/gtx/component_wise.hpp>

#include "../samplers/sampling.hpp"
#include "material.hpp"

template <typename T>
inline T F(float w_i_h, T k_s) {
  using namespace glm;
  return k_s + (1.f - k_s) * pow(1.f - abs(w_i_h), 5.f);
}

inline float D(float h_n, float a) {
  using namespace glm;
  return ONE_OVER_PI / pow(a * pow(h_n, 2.f) + (1 - pow(h_n, 2.f)) / a, 2.f);
}

inline float G1(float v_n, float a) {
  using namespace glm;
  return 2 / (1 + sqrt(1 + pow(a / v_n, 2.f) - pow(a, 2.f)));
}

inline float G(float w_i_n, float w_o_n, float a) {
  return G1(w_i_n, a) * G1(w_o_n, a);
}

inline glm::vec3 SampleHalfVector(const glm::vec3 &n, float a) {
  using namespace glm;
  const auto xi_1 = Random(), xi_2 = Random();
  const auto theta = atan(a * sqrt(xi_1 / (1 - xi_1))), phi = TWO_PI * xi_2;
  return ConvertSpherical(theta, phi, n);
}

class GGXReflection : public Material {
 public:
  GGXReflection(const glm::vec3 &k_s, float alpha) : alpha_(alpha), k_s_(k_s) {}

  GGXReflection(float n, float alpha)
      : alpha_(alpha), k_s_(glm::pow((n - 1) / (n + 1), 2.f)) {}

  friend bool operator==(const GGXReflection &x, const GGXReflection &y) {
    return x.k_s_ == y.k_s_ && x.alpha_ == y.alpha_;
  }

  friend bool operator!=(const GGXReflection &x, const GGXReflection &y) {
    return !(x == y);
  }

  glm::vec3 Brdf(const glm::vec3 &n, const glm::vec3 &w_i,
                 const glm::vec3 &w_o) const override {
    using namespace glm;

    const auto w_i_n = dot(w_i, n), w_o_n = dot(w_o, n);
    const auto h = sign(w_i_n) * normalize(w_i + w_o);
    const auto h_n = dot(h, n), w_i_h = dot(w_i, h), w_o_h = dot(w_o, h);
    if (h_n > 0 && w_i_h * w_i_n > 0 && w_o_h * w_o_n > 0) {
      return F(w_i_h, k_s_) * G(w_i_n, w_o_n, alpha_) * D(h_n, alpha_) /
             (4 * abs(w_i_n * w_o_n));
    } else
      return vec3{0.f};
  }

  float Pdf(const glm::vec3 &n, const glm::vec3 &w_i,
            const glm::vec3 &w_o) const override {
    using namespace glm;

    const auto h = sign(dot(w_i, n)) * normalize(w_i + w_o);
    const auto h_n = dot(h, n);
    if (h_n > 0)
      return D(h_n, alpha_) * h_n / (4 * abs(dot(h, w_o)));
    else
      return 0.f;
  }

  glm::vec3 Sample(const glm::vec3 &n, const glm::vec3 &w_i) const override {
    using namespace glm;
    const auto h = SampleHalfVector(n, alpha_);
    return reflect(-w_i, h);
  }

  float Power(const glm::vec3 &n, const glm::vec3 &w_i) const override {
    return glm::compAdd(F(glm::dot(w_i, n), k_s_));
  }

 private:
  float alpha_;
  glm::vec3 k_s_;
};

class GGXRefraction : public Material {
 public:
  GGXRefraction(float n, float alpha)
      : alpha_(alpha), n_(n), k_s_(glm::pow((n - 1) / (n + 1), 2.f)) {}

  friend bool operator==(const GGXRefraction &x, const GGXRefraction &y) {
    return x.n_ == y.n_ && x.alpha_ == y.alpha_;
  }

  friend bool operator!=(const GGXRefraction &x, const GGXRefraction &y) {
    return !(x == y);
  }

  glm::vec3 Brdf(const glm::vec3 &n, const glm::vec3 &w_i,
                 const glm::vec3 &w_o) const override {
    using namespace glm;

    const auto w_i_n = dot(w_i, n), w_o_n = dot(w_o, n);
    const auto eta = RefractionRatio(w_i_n);
    const auto h = -normalize(eta * w_i + w_o);
    const auto h_n = dot(h, n), w_i_h = dot(w_i, h), w_o_h = dot(w_o, h);
    if (h_n > 0 && w_i_h * w_i_n > 0 && w_o_h * w_o_n > 0) {
      return vec3{abs(w_i_h * w_o_h) * (1.f - F(w_i_h, k_s_)) *
                  G(w_i_n, w_o_n, alpha_) * D(h_n, alpha_) /
                  abs(w_i_n * w_o_n) / pow(eta * w_i_h + w_o_h, 2.f)};
    }
    return vec3{0.f};
  }

  float Pdf(const glm::vec3 &n, const glm::vec3 &w_i,
            const glm::vec3 &w_o) const override {
    using namespace glm;

    const auto eta = RefractionRatio(dot(w_i, n));
    const auto h = -normalize(eta * w_i + w_o);
    const auto h_n = dot(h, n), w_i_h = dot(w_i, h), w_o_h = dot(w_o, h);
    if (h_n > 0) {
      return D(h_n, alpha_) * abs(h_n * w_o_h) / pow(eta * w_i_h + w_o_h, 2.f);
    } else
      return 0.f;
  }

  glm::vec3 Sample(const glm::vec3 &n, const glm::vec3 &w_i) const override {
    using namespace glm;
    const auto h = SampleHalfVector(n, alpha_);
    return refract(-w_i, sign(dot(w_i, h)) * h, RefractionRatio(dot(w_i, n)));
  }

  float Power(const glm::vec3 &n, const glm::vec3 &w_i) const override {
    return 1 - F(glm::dot(w_i, n), k_s_);
  }

 private:
  float RefractionRatio(float v_n) const {
    const auto eta_i = v_n > 0 ? 1.f : n_;
    const auto eta_o = v_n > 0 ? n_ : 1.f;
    return eta_i / eta_o;
  }

  float alpha_;
  float n_;
  float k_s_;
};