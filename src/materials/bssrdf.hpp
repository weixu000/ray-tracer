#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "../ray.hpp"
#include "../samplers/sampling.hpp"

struct BSSRDF {
  BSSRDF(float A = .8f, float n = 1.5f)
      : A(A),
        R_0(glm::pow((n - 1) / (n + 1), 2)),
        d(1 / (1.85 - A + 7 * glm::pow(glm::abs(A - .8f), 3.f))) {}

  glm::vec3 Value(const glm::vec3 &p_i, const glm::vec3 &n_i,
                  const glm::vec3 &w_i, const glm::vec3 &p_o,
                  const glm::vec3 &n_o, const glm::vec3 &w_o) const {
    using namespace glm;
    const auto w_i_n = dot(w_i, n_i), w_o_n = dot(w_o, n_o);
    if (w_i_n > 0 && w_o_n > 0)
      return vec3{(1 - F(w_o_n)) * A * R(length(p_i - p_o)) * (1 - F(w_i_n)) /
                  (PI * (1 - R_0) * 20 / 21)};
    else
      return vec3{0.f};
  }

  template <typename F>
  std::optional<std::tuple<RayHit, float>> SamplePi(const glm::vec3 &p_o,
                                                    const glm::vec3 &n_o,
                                                    F &&hit_func) const {
    using namespace glm;
    const auto s = SampleR();
    const auto frame = mat3(orientation(n_o, {0, 0, 1}));
    const auto half_l = sqrt(pow(10 * d, 2.f) - length2(s));

    thread_local std::vector<RayHit> hits;
    hit_func(hits, Ray{frame * vec3{s, half_l} + p_o, -n_o}, 0.f, 2 * half_l);
    if (hits.empty())
      return std::nullopt;
    else {
      const auto hit = hits[size_t(Random() * hits.size())];
      return std::make_tuple(
          hit, PdfR(length(s)) / hits.size() * abs(dot(n_o, hit.n)));
    }
  }

  glm::vec3 SampleWi(const glm::vec3 &n_i) const {
    using namespace glm;

    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(sqrt(xi_1)), phi = TWO_PI * xi_2;
    return ConvertSpherical(theta, phi, n_i);
  }

  float PdfWi(const glm::vec3 &n_i, const glm::vec3 &w_i) const {
    using namespace glm;
    return dot(n_i, w_i) / PI;
  }

 private:
  float F(float v_n) const {
    using namespace glm;
    return R_0 + (1.f - R_0) * pow(1.f - v_n, 5.f);
  }

  float R(float r) const {
    using namespace glm;
    return (exp(-r / d) + exp(-r / (3 * d))) / (8 * PI * d * r);
  }

  glm::vec2 SampleR() const {
    using namespace glm;
    const auto dd = Random() < .25f ? d : 3 * d;
    const auto r = -dd * log(1 - Random());
    const auto phi = TWO_PI * Random();
    return vec2{r * cos(phi), r * sin(phi)};
  }

  float PdfR(float r) const { return R(r); }

  float A;
  float R_0;
  float d;
};
