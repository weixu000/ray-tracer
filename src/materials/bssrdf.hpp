#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "../ray.hpp"
#include "../samplers/sampling.hpp"

struct BSSRDF {
  glm::vec3 Value(const glm::vec3 &p_i, const glm::vec3 &n_i,
                  const glm::vec3 &w_i, const glm::vec3 &p_o,
                  const glm::vec3 &n_o, const glm::vec3 &w_o) const {
    using namespace glm;
    const auto w_i_n = dot(w_i, n_i), w_o_n = dot(w_o, n_o);
    if (w_i_n > 0 && w_o_n > 0)
      return vec3{(1 - F(w_o_n)) * A * R(length(p_i - p_o)) * (1 - F(w_i_n)) *
                  ONE_OVER_PI};
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
    const auto half_l = sqrt(pow(r_max, 2.f) - length2(s));
    const auto &hits =
        hit_func(Ray{frame * vec3{s, half_l} + p_o, -n_o}, 0.f, 2 * half_l);
    if (hits.empty())
      return std::nullopt;
    else {
      return std::make_tuple(hits[size_t(Random() * hits.size())],
                             PdfR(length(s)) / hits.size());
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

  float A = .8f;
  float n = 1.5f;
  float k_s = glm::pow((n - 1) / (n + 1), 2);
  float d = 1 / (1.85 - A + 7 * glm::pow(glm::abs(A - .8f), 3.f));
  float r_max = 10 * d;

 private:
  float F(float v_n) const {
    using namespace glm;
    return k_s + (1.f - k_s) * pow(1.f - v_n, 5.f);
  }

  float R(float r) const {
    using namespace glm;
    return (exp(-r / d) + exp(-r / (3 * d))) / (8 * PI * d * r);
  }

  glm::vec2 SampleR() const {
    using namespace glm;

    const auto w_1 = 1 - exp(-r_max / d), w_2 = 3 * (1 - exp(-r_max / (3 * d)));
    const auto dd = Random() < w_1 / (w_1 + w_2) ? d : 3 * d;
    const auto r = -dd * log(1 - Random() * (1 - exp(-r_max / dd)));
    const auto phi = TWO_PI * Random();
    return vec2{r * cos(phi), r * sin(phi)};
  }

  float PdfR(float r) const {
    using namespace glm;
    const auto w_1 = 1 - exp(-r_max / d), w_2 = 3 * (1 - exp(-r_max / (3 * d)));
    return R(r) / (w_1 + w_2) * 4;
  }
};
