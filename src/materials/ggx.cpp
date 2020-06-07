#include "ggx.hpp"

using namespace glm;

namespace {
template <typename T>
T F(float w_i_h, T k_s) {
  return k_s + (1.f - k_s) * pow(1.f - abs(w_i_h), 5.f);
}

float D(float h_n, float a) {
  return ONE_OVER_PI / pow(a * pow(h_n, 2.f) + (1 - pow(h_n, 2.f)) / a, 2.f);
}

float G1(float v_n, float a) {
  return 2 / (1 + sqrt(1 + pow(a / v_n, 2.f) - pow(a, 2.f)));
}

float G(float w_i_n, float w_o_n, float a) {
  return G1(w_i_n, a) * G1(w_o_n, a);
}

vec3 SampleHalfVector(const vec3& n, float a) {
  const auto xi_1 = Random(), xi_2 = Random();
  const auto theta = atan(a * sqrt(xi_1 / (1 - xi_1))), phi = TWO_PI * xi_2;
  return ConvertSpherical(theta, phi, n);
}

float RefractionRatio(float v_n, float eta) {
  const auto eta_i = v_n > 0 ? 1.f : eta;
  const auto eta_o = v_n > 0 ? eta : 1.f;
  return eta_i / eta_o;
}
}  // namespace

template class GGXBSDF<true>;
template class GGXBSDF<false>;

template <bool transmission>
vec3 GGXBSDF<transmission>::Value(const vec3& w_i, const vec3& w_o) const {
  const auto w_i_n = dot(w_i, n), w_o_n = dot(w_o, n);

  vec3 bsdf{0.f};
  {
    const auto h = sign(w_i_n) * normalize(w_i + w_o);
    const auto h_n = dot(h, n), w_i_h = dot(w_i, h), w_o_h = dot(w_o, h);
    if (h_n > 0 && w_i_h * w_i_n > 0 && w_o_h * w_o_n > 0) {
      bsdf += F(w_i_h, k_s) * G(w_i_n, w_o_n, alpha) * D(h_n, alpha) /
              (4 * abs(w_i_n * w_o_n));
    }
  }
  if constexpr (transmission) {
    for (size_t c = 0; c < 3; ++c) {
      const auto eta = RefractionRatio(w_i_n, ior[c]);
      const auto h = -normalize(eta * w_i + w_o);
      const auto h_n = dot(h, n), w_i_h = dot(w_i, h), w_o_h = dot(w_o, h);
      if (w_i_h * w_o_h < 0 && h_n > 0 && w_i_h * w_i_n > 0 &&
          w_o_h * w_o_n > 0) {
        bsdf[c] += abs(w_i_h * w_o_h) * (1.f - F(w_i_h, k_s[c])) *
                   G(w_i_n, w_o_n, alpha) * D(h_n, alpha) / abs(w_i_n * w_o_n) /
                   pow(eta * w_i_h + w_o_h, 2.f);
      }
    }
  }

  return bsdf;
}

template <bool transmission>
vec3 GGXBSDF<transmission>::Sample(const vec3& w_i) const {
  const auto h = SampleHalfVector(n, alpha);

  if constexpr (transmission) {
    if (Random() < compAdd(F(dot(w_i, h), k_s)) / 3) {
      return reflect(-w_i, h);
    } else {
      const auto c = size_t(Random() * 3);
      return refract(-w_i, sign(dot(w_i, h)) * h,
                     RefractionRatio(dot(w_i, n), ior[c]));
    }
  } else {
    return reflect(-w_i, h);
  }
}

template <bool transmission>
float GGXBSDF<transmission>::Pdf(const vec3& w_i, const vec3& w_o) const {
  auto pdf = 0.f;
  {
    const auto h = normalize(w_i + w_o);
    const auto h_n = abs(dot(h, n));
    const auto t = transmission ? compAdd(F(dot(w_i, h), k_s)) / 3 : 1.f;
    pdf += D(h_n, alpha) * h_n / (4 * abs(dot(h, w_o))) * t;
  }

  if constexpr (transmission) {
    for (size_t c = 0; c < 3; ++c) {
      const auto eta = RefractionRatio(dot(w_i, n), ior[c]);
      const auto h = -normalize(eta * w_i + w_o);
      const auto w_i_h = dot(w_i, h), w_o_h = dot(w_o, h);
      if (w_i_h * w_o_h < 0) {
        const auto h_n = abs(dot(h, n));
        const auto t = compAdd(F(dot(w_i, h), k_s)) / 3;
        pdf += D(h_n, alpha) * h_n * abs(w_o_h) /
               pow(eta * w_i_h + w_o_h, 2.f) * (1 - t) / 3;
      }
    }
  }
  return pdf;
}

template <bool transmission>
float GGXBSDF<transmission>::Weight(const vec3& w_i) const {
  return glm::compAdd(F(dot(w_i, n), k_s));
}
