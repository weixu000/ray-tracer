#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "RayTracer/Camera.hpp"
#include "RayTracer/Light.hpp"
#include "RayTracer/Ray.hpp"
#include <RayTracer/Primitive.hpp>

class Scene {
public:
  int width, height;
  int max_depth = 5;
  std::string output_file;
  Camera camera;

  int max_num_verts;
  std::vector<glm::vec3> verts;

  std::vector<Primitive> primitives;

  std::vector<std::unique_ptr<Light>> lights;

  std::optional<RayHit> Trace(const Ray &ray) const;

  glm::vec3 Shade(const Ray &ray, int depth) const;
};