#pragma once

#include <glm/glm.hpp>

#include "../camera.hpp"
#include "../image.hpp"
#include "../scene.hpp"

/**
 * Use camera to render the scene
 */
class Integrator {
 public:
  Integrator(const Scene &scene, const Camera &camera, float gamma)
      : scene_(scene), camera_(camera), gamma_(gamma) {}

  virtual ~Integrator() = default;

  Image Render() const;

 protected:
  const Scene &scene_;
  const Camera &camera_;

 private:
  virtual glm::vec3 ShadePixel(const glm::vec2 &pixel) const = 0;

  float gamma_;
};
