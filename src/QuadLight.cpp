#include "raytracer/lights/QuadLight.hpp"

using namespace glm;

QuadLight::QuadLight(const glm::vec3 &intensity, const glm::vec3 &v0,
                     const glm::vec3 &e1, const glm::vec3 &e2)
    : Light(intensity), v0_(v0), e1_(e1), e2_(e2) {
  v_[0] = v0;
  v_[1] = v0 + e1;
  v_[2] = v_[1] + e2;
  v_[3] = v0 + e2;
  normal_ = cross(e1, e2);
  area_ = length(normal_);
}

LightSample QuadLight::GenerateSample(const glm::vec3 &incident,
                                      const glm::vec2 &uv) const {
  const auto p = v0_ + uv.x * e1_ + uv.y * e2_;
  const auto d = p - incident;
  return LightSample{p, incident, intensity, normal_, area_};
}

std::optional<float> QuadLight::Hit(const Ray &ray) const {
  const auto o = ray.origin, d = ray.direction;
  const auto res = inverse(mat3(e1_, e2_, -d)) * (o - v0_);
  const auto u = res[0], v = res[1], t = res[2];

  if (u >= 0 && u <= 1 && v >= 0 && v <= 1 && t >= 0) {
    return t;
  } else {
    return std::nullopt;
  }
}

glm::vec3 QuadLight::GetIrradianceVector(const glm::vec3 &r) const {
  glm::vec3 e[4];
  for (int i = 0; i < 4; ++i) {
    e[i] = normalize(v_[i] - r);
  }
  float theta[4];
  glm::vec3 gamma[4];
  for (int i = 0; i < 4; ++i) {
    theta[i] = acos(dot(e[i], e[(i + 1) % 4]));
    gamma[i] = normalize(cross(e[i], e[(i + 1) % 4]));
  }

  glm::vec3 phi(0.f);
  for (int i = 0; i < 4; ++i) {
    phi += theta[i] * gamma[i];
  }
  return phi / 2.f;
}
