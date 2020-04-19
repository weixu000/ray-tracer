#pragma once

#include <glm/glm.hpp>

#include <raytracer/camera.hpp>
#include <raytracer/image.hpp>
#include <raytracer/scene.hpp>

/**
 * Use camera to render the scene
 */
class Integrator {
public:
  Integrator(const Scene &scene, const Camera &camera)
      : scene_(scene), camera_(camera) {}

  virtual ~Integrator() = default;

  Image Render() const;

protected:
  const Scene &scene_;
  const Camera &camera_;

private:
  virtual glm::vec3 ShadePixel(const glm::vec2 &pixel) const = 0;
};
