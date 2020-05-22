#include "quad_light.hpp"

#include "../samplers/sampling.hpp"

using namespace glm;

QuadLight::QuadLight(const glm::vec3 &radiance, const glm::vec3 &v0,
                     const glm::vec3 &e1, const glm::vec3 &e2)
    : radiance_(radiance), v0_(v0), e1_(e1), e2_(e2) {
  v_[0] = v0;
  v_[1] = v0 + e1;
  v_[2] = v_[1] + e2;
  v_[3] = v0 + e2;
  normal_ = normalize(cross(e1, e2));
  area_ = length(cross(e1, e2));
}

glm::vec3 QuadLight::Sample() const {
  const auto p = v0_ + Random() * e1_ + Random() * e2_;
  return p;
}

std::optional<Emission> QuadLight::Hit(const Ray &ray) const {
  const auto o = ray.origin, d = ray.direction;
  const auto res = inverse(mat3(e1_, e2_, -d)) * (o - v0_);
  const auto u = res[0], v = res[1], t = res[2];

  if (u >= 0 && u <= 1 && v >= 0 && v <= 1 && t >= 0) {
    return Emission{t, dot(d, normal_) > 0 ? radiance_ : vec3{0.f}, normal_,
                    area_, this};
  } else {
    return std::nullopt;
  }
}
