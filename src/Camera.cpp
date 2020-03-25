#include "RayTracer/Camera.h"

Camera::Camera(const glm::vec3 &look_from, const glm::vec3 &look_at,
               const glm::vec3 &up, float fov)
    : tan_fov_2_(glm::tan(glm::radians(fov / 2))), look_from_(look_from) {
  using namespace glm;
  const auto f = normalize(look_at - look_from);
  const auto s = normalize(cross(f, up));
  const auto u(cross(s, f));
  view_ = mat3(s, u, -f);
}

Ray Camera::GenerateEyeRay(const glm::vec2 &pixel) const {
  return {look_from_,
          view_ * glm::vec3(pixel.x * tan_fov_2_, pixel.y * tan_fov_2_, -1.f)};
}
