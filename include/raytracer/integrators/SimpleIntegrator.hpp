#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "raytracer/Camera.hpp"
#include "raytracer/Scene.hpp"

class SimpleIntegrator {
public:
  explicit SimpleIntegrator(const Scene &scene, const Camera &camera,
                            int max_depth = 5);

  std::vector<glm::u8vec3> Render() const;

private:
  glm::vec3 Shade(const Ray &ray, int depth) const;

  const Scene &scene_;
  const Camera &camera_;
  int max_depth_;
};
