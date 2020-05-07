#pragma once

#include "ray.hpp"

class Camera {
public:
  Camera() = default;
  Camera(const glm::vec3 &look_from, const glm::vec3 &look_at,
         const glm::vec3 &up, float fov, int w, int h);

  //
  /**
   * Pixel coordinates normalized by height
   * Direction of ray is normalized
   */
  Ray GenerateEyeRay(const glm::vec2 &pixel) const;

  int width_, height_;

private:
  float tan_fov_2_;
  glm::mat3 view_;
  glm::vec3 look_from_;
};