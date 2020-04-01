#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "raytracer/Camera.hpp"
#include "raytracer/Scene.hpp"

class Integrator {
public:
  Integrator(const Scene &scene, const Camera &camera);

  std::vector<glm::u8vec3> Render() const;

protected:
  const Scene &scene_;

private:
  virtual glm::vec3 Shade(const Ray &ray) const = 0;

  const Camera &camera_;
};
