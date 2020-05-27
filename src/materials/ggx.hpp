#pragma once

#include <glm/gtx/component_wise.hpp>

#include "../samplers/sampling.hpp"
#include "composed_bsdf.hpp"
#include "lambertian.hpp"

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

struct GGXBRDF {
  glm::vec3 Value(const glm::vec3 &w_i, const glm::vec3 &w_o) const {
    using namespace glm;

    const auto w_i_n = dot(w_i, n), w_o_n = dot(w_o, n);
    const auto h = sign(w_i_n) * normalize(w_i + w_o);
    const auto h_n = dot(h, n), w_i_h = dot(w_i, h), w_o_h = dot(w_o, h);
    if (h_n > 0 && w_i_h * w_i_n > 0 && w_o_h * w_o_n > 0) {
      return F(w_i_h, k_s) * G(w_i_n, w_o_n, alpha) * D(h_n, alpha) /
             (4 * abs(w_i_n * w_o_n));
    } else
      return vec3{0.f};
  }

  float Pdf(const glm::vec3 &w_i, const glm::vec3 &w_o) const {
    using namespace glm;

    const auto h = sign(dot(w_i, n)) * normalize(w_i + w_o);
    const auto h_n = dot(h, n);
    if (h_n > 0)
      return D(h_n, alpha) * h_n / (4 * abs(dot(h, w_o)));
    else
      return 0.f;
  }

  glm::vec3 Sample(const glm::vec3 &w_i) const {
    using namespace glm;
    const auto h = SampleHalfVector(n, alpha);
    return reflect(-w_i, h);
  }

  float Weight(const glm::vec3 &w_i) const {
    return glm::compAdd(F(glm::dot(w_i, n), k_s));
  }

  glm::vec3 n;
  float alpha;
  glm::vec3 k_s;
};

class GGXReflection {
 public:
  GGXReflection(const glm::vec3 &k_d, const glm::vec3 &k_s, float alpha)
      : k_d_(k_d), alpha_(alpha), k_s_(k_s) {}

  GGXReflection(const glm::vec3 &k_d, float n, float alpha)
      : k_d_(k_d), alpha_(alpha), k_s_(glm::pow((n - 1) / (n + 1), 2.f)) {}

  auto GetBxDF(const glm::vec3 &n) const {
    ComposedBSDF<LambertianBRDF, GGXBRDF> bsdf;

    std::get<0>(bsdf.bsdfs).n = n;
    std::get<0>(bsdf.bsdfs).k_d = k_d_;

    std::get<1>(bsdf.bsdfs).n = n;
    std::get<1>(bsdf.bsdfs).alpha = alpha_;
    std::get<1>(bsdf.bsdfs).k_s = k_s_;

    return bsdf;
  }

 private:
  float alpha_;
  glm::vec3 k_s_;
  glm::vec3 k_d_;
};

struct GGXBTDF {
  glm::vec3 Value(const glm::vec3 &w_i, const glm::vec3 &w_o) const {
    using namespace glm;

    const auto w_i_n = dot(w_i, n), w_o_n = dot(w_o, n);
    const auto eta = RefractionRatio(w_i_n);
    const auto h = -normalize(eta * w_i + w_o);
    const auto h_n = dot(h, n), w_i_h = dot(w_i, h), w_o_h = dot(w_o, h);
    if (h_n > 0 && w_i_h * w_i_n > 0 && w_o_h * w_o_n > 0) {
      return vec3{abs(w_i_h * w_o_h) * (1.f - F(w_i_h, k_s)) *
                  G(w_i_n, w_o_n, alpha) * D(h_n, alpha) / abs(w_i_n * w_o_n) /
                  pow(eta * w_i_h + w_o_h, 2.f)};
    }
    return vec3{0.f};
  }

  float Pdf(const glm::vec3 &w_i, const glm::vec3 &w_o) const {
    using namespace glm;

    const auto eta = RefractionRatio(dot(w_i, n));
    const auto h = -normalize(eta * w_i + w_o);
    const auto h_n = dot(h, n), w_i_h = dot(w_i, h), w_o_h = dot(w_o, h);
    if (h_n > 0) {
      return D(h_n, alpha) * abs(h_n * w_o_h) / pow(eta * w_i_h + w_o_h, 2.f);
    } else
      return 0.f;
  }

  glm::vec3 Sample(const glm::vec3 &w_i) const {
    using namespace glm;
    const auto h = SampleHalfVector(n, alpha);
    return refract(-w_i, sign(dot(w_i, h)) * h, RefractionRatio(dot(w_i, n)));
  }

  float Weight(const glm::vec3 &w_i) const {
    return 1 - F(glm::dot(w_i, n), k_s);
  }

  glm::vec3 n;
  float alpha;
  float ior;
  float k_s;

 private:
  float RefractionRatio(float v_n) const {
    const auto eta_i = v_n > 0 ? 1.f : ior;
    const auto eta_o = v_n > 0 ? ior : 1.f;
    return eta_i / eta_o;
  }
};

class GGXRefraction {
 public:
  GGXRefraction(float n, float alpha)
      : alpha_(alpha), ior_(n), k_s_(glm::pow((n - 1) / (n + 1), 2.f)) {}

  auto GetBxDF(const glm::vec3 &n) const {
    GGXBTDF bsdf;
    bsdf.n = n;
    bsdf.alpha = alpha_;
    bsdf.ior = ior_;
    bsdf.k_s = k_s_;

    return bsdf;
  }

 private:
  float alpha_;
  float ior_;
  float k_s_;
};