#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "../samplers/phong_sampler.hpp"

struct Phong {
  glm::vec3 Brdf(const glm::vec3 &n, const glm::vec3 &w_i,
                 const glm::vec3 &w_o) const {
    using namespace glm;

    const auto r = reflect(-w_o, n);
    const auto f = (k_d + k_s * (s + 2) / 2.f * pow(max(0.f, dot(r, w_i)), s)) *
                   one_over_pi<float>();
    return f;
  }

  std::tuple<glm::vec3, float> SampleBrdf(const glm::vec3 &n,
                                          const glm::vec3 &w_o) const {
    static PhongSampler sampler;
    return sampler.Sample(k_d, k_s, s, n, w_o);
  }

  glm::vec3 k_d, k_s;
  float s;
};