#pragma once

#include <glm/gtx/component_wise.hpp>

#include "../samplers/sampling.hpp"
#include "material.hpp"

class Lambertian : public Material {
 public:
  Lambertian(const glm::vec3 &k_d) : k_d_(k_d) {}

  friend bool operator==(const Lambertian &x, const Lambertian &y) {
    return x.k_d_ == y.k_d_;
  }

  friend bool operator!=(const Lambertian &x, const Lambertian &y) {
    return !(x == y);
  }

  glm::vec3 Brdf(const glm::vec3 &n, const glm::vec3 &w_i,
                 const glm::vec3 &w_o) const override {
    if (dot(w_i, n) > 0)
      return k_d_ * ONE_OVER_PI;
    else
      return glm::vec3{0.f};
  }

  glm::vec3 Sample(const glm::vec3 &n, const glm::vec3 &w_i) const override {
    using namespace glm;
    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(sqrt(xi_1)), phi = TWO_PI * xi_2;
    return ConvertSpherical(theta, phi, n);
  }

  float Pdf(const glm::vec3 &n, const glm::vec3 &w_i,
            const glm::vec3 &w_o) const override {
    using namespace glm;
    return max(0.f, dot(n, w_o)) * ONE_OVER_PI;
  }

  float Power(const glm::vec3 &n, const glm::vec3 &w_i) const override {
    return glm::compAdd(k_d_);
  }

 private:
  glm::vec3 k_d_;
};
