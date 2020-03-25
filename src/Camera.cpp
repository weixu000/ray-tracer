#include "RayTracer/Camera.hpp"

static glm::mat3 ComputeViewMatrix(const glm::vec3 &look_from,
                                   const glm::vec3 &look_at,
                                   const glm::vec3 &up) {
  using namespace glm;
  const auto f = normalize(look_at - look_from);
  const auto s = normalize(cross(f, up));
  const auto u(cross(s, f));
  return mat3(s, u, -f);
}

Camera::Camera(const glm::vec3 &look_from, const glm::vec3 &look_at,
               const glm::vec3 &up, float fov)
    : tan_fov_2_(glm::tan(glm::radians(fov / 2))),
      view_(ComputeViewMatrix(look_from, look_at, up)), look_from_(look_from) {}

Ray Camera::GenerateEyeRay(const glm::vec2 &pixel) const {
  return {look_from_,
          view_ * glm::vec3(pixel.x * tan_fov_2_, pixel.y * tan_fov_2_, -1.f)};
}
