#pragma once
#include <glm/glm.hpp>

#include "RayTracer/Ray.h"

class Camera {
public:
  Camera() = default;
  Camera(const glm::vec3 &look_from, const glm::vec3 &look_at,
         const glm::vec3 &up, float fov);

  // Pixel coordinates normalized by height
  Ray GenerateEyeRay(const glm::vec2 &pixel) const;

private:
  float tan_fov_2_;
  glm::mat3 view_;
  glm::vec3 look_from_;
};