#include "bssrdf.hpp"

using namespace glm;

float NormalizedDiffusionProfile::R(float r) const {
  return (exp(-r / d) + exp(-r / (3 * d))) / (8 * PI * d * r);
}

vec2 NormalizedDiffusionProfile::SampleR() const {
  const auto dd = Random() < .25f ? d : 3 * d;
  const auto r = -dd * log(1 - Random());
  const auto phi = TWO_PI * Random();
  return vec2{r * cos(phi), r * sin(phi)};
}

vec3 BSSRDF::Value(const vec3& p_i, const vec3& n_i, const vec3& w_i,
                   const vec3& p_o, const vec3& n_o, const vec3& w_o) const {
  const auto w_i_n = dot(w_i, n_i), w_o_n = dot(w_o, n_o);
  if (w_i_n > 0 && w_o_n > 0)
    return vec3{(1 - F(w_o_n)) * A * profile.R(length(p_i - p_o)) *
                (1 - F(w_i_n)) / (PI * (1 - R_0) * 20 / 21)};
  else
    return vec3{0.f};
}

float BSSRDF::F(float v_n) const {
  return R_0 + (1.f - R_0) * pow(1.f - v_n, 5.f);
}

std::optional<std::tuple<RayHit, float>> BSSRDF::SamplePi(
    const vec3& p_o, const vec3& n_o, const BSSRDF::ProbeFunc& hit_func) const {
  using namespace glm;
  const auto s = profile.SampleR();
  const auto frame = mat3(orientation(n_o, {0, 0, 1}));
  const auto half_l = sqrt(pow(profile.r_max, 2.f) - length2(s));
  const auto prob_ray = Ray{frame * vec3{s, half_l} + p_o, -n_o};

  thread_local std::vector<RayHit> hits;
  hit_func(hits, prob_ray, 0.f, 2 * half_l);
  if (hits.empty())
    return std::nullopt;
  else {
    const auto hit = hits[size_t(Random() * hits.size())];
    return std::make_tuple(
        hit, profile.PdfR(length(s)) / hits.size() * abs(dot(n_o, hit.n)));
  }
}
