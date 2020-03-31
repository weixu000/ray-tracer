#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "raytracer/Camera.hpp"
#include "raytracer/Scene.hpp"

class Integrator {
public:
  Integrator(const Scene &scene, const Camera &camera);

  virtual std::vector<glm::u8vec3> Render() const = 0;

protected:
  const Scene &scene_;
  const Camera &camera_;
};
