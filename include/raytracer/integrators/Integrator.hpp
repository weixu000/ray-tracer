#pragma once

#include <glm/glm.hpp>

#include "raytracer/Camera.hpp"
#include "raytracer/Image.hpp"
#include "raytracer/Scene.hpp"

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

private:
  virtual glm::vec3 Shade(const Ray &ray) const = 0;

  const Camera &camera_;
};
