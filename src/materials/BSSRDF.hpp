#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "../ray.hpp"
#include "../samplers/sampling.hpp"

class BSSRDF {
 public:
  glm::vec3 Value(const glm::vec3 &p_i, const glm::vec3 &n_i,
                  const glm::vec3 &w_i, const glm::vec3 &p_o,
                  const glm::vec3 &n_o, const glm::vec3 &w_o) const {
    using namespace glm;
    const auto w_i_n = dot(w_i, n_i), w_o_n = dot(w_o, n_o);
    if (w_i_n > 0 && w_o_n > 0)
      return vec3{(1 - F(w_o_n)) * A_ * R(length(p_i - p_o)) * (1 - F(w_i_n)) *
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
    const auto half_l = sqrt(pow(r_max_, 2.f) - length2(s));
    const auto &hits =
        hit_func(Ray{frame * vec3{s, half_l} + p_o, -n_o}, 0.f, 2 * half_l);
    if (hits.empty())
      return std::nullopt;
    else {
      return std::make_tuple(hits[size_t(Random() * hits.size())],
                             PdfR(length(s)) / hits.size());
    }
  }

 private:
  float F(float v_n) const {
    using namespace glm;
    return k_s_ + (1.f - k_s_) * pow(1.f - v_n, 5.f);
  }

  float R(float r) const {
    using namespace glm;
    return (exp(-r / d_) + exp(-r / (3 * d_))) / (8 * PI * d_ * r);
  }

  glm::vec2 SampleR() const {
    using namespace glm;

    const auto w_1 = 1 - exp(-r_max_ / d_),
               w_2 = 3 * (1 - exp(-r_max_ / (3 * d_)));
    const auto dd = Random() < w_1 / (w_1 + w_2) ? d_ : 3 * d_;
    const auto r = -dd * log(1 - Random() * (1 - exp(-r_max_ / dd)));
    const auto phi = TWO_PI * Random();
    return vec2{r * cos(phi), r * sin(phi)};
  }

  float PdfR(float r) const {
    using namespace glm;
    const auto w_1 = 1 - exp(-r_max_ / d_),
               w_2 = 3 * (1 - exp(-r_max_ / (3 * d_)));
    return R(r) / (w_1 + w_2) * 4;
  }

  float A_ = .8f;
  float n_ = 1.5f;
  float k_s_ = glm::pow((n_ - 1) / (n_ + 1), 2);
  float d_ = 1 / (1.85 - A_ + 7 * glm::pow(glm::abs(A_ - .8f), 3.f));
  float r_max_ = 10 * d_;
};
