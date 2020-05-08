#pragma once

#include <glm/gtx/component_wise.hpp>

#include "random.hpp"
#include "sampler.hpp"

class PhongSampler : public Sampler {
 public:
  std::tuple<glm::vec3, float> Sample(const BRDF& brdf,
                                      const glm::vec3& w_o) const override {
    using namespace glm;

    const auto n = brdf.normal;
    const auto s = brdf.shininess;
    const auto t =
        compMax(brdf.specular) / compMax(brdf.diffuse + brdf.specular);
    const auto r = reflect(-w_o, n);
    const auto w_i = Random() <= t ? SampleSpecular(r, s) : SampleDiffuse(n);
    const auto pdf = (1 - t) * max(0.f, dot(n, w_i)) / PI +
                     t * (s + 1) / (2 * PI) * pow(max(0.f, dot(r, w_i)), s);
    return std::make_tuple(w_i, pdf);
  }

 private:
  /**
   * Sampling specular component
   */
  glm::vec3 SampleSpecular(const glm::vec3& r, const float s) const {
    using namespace glm;
    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(pow(xi_1, 1 / (s + 1))), phi = 2 * PI * xi_2;
    return ConvertSpherical(theta, phi, r);
  }

  /**
   * Sampling diffuse component
   */
  glm::vec3 SampleDiffuse(const glm::vec3& n) const {
    using namespace glm;
    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(sqrt(xi_1)), phi = 2 * PI * xi_2;
    return ConvertSpherical(theta, phi, n);
  }
};
