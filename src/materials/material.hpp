#pragma once

#include <glm/glm.hpp>

#include "../samplers/sampling.hpp"

class Material {
 public:
  Material(const glm::vec3 &k_d, const glm::vec3 &k_s) : k_d_(k_d), k_s_(k_s) {}

  virtual ~Material() = default;

  virtual glm::vec3 Brdf(const glm::vec3 &n, const glm::vec3 &w_i,
                         const glm::vec3 &w_o) const = 0;

  virtual glm::vec3 Sample(const glm::vec3 &n, const glm::vec3 &w_o) const = 0;

  virtual float Pdf(const glm::vec3 &n, const glm::vec3 &w_i,
                    const glm::vec3 &w_o) const = 0;

 protected:
  glm::vec3 DiffuseBrdf() const { return k_d_ * ONE_OVER_PI; }

  glm::vec3 SampleDiffuse(const glm::vec3 &n) const {
    using namespace glm;
    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(sqrt(xi_1)), phi = TWO_PI * xi_2;
    return ConvertSpherical(theta, phi, n);
  }

  glm::vec3 k_d_, k_s_;
};
