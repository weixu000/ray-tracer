#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "RayTracer/Camera.h"
#include "RayTracer/Geometry.h"
#include "RayTracer/Light.h"
#include "RayTracer/Material.h"
#include "RayTracer/Ray.h"

class Scene {
public:
  int width, height;
  int max_depth = 5;
  std::string output_file;
  Camera camera;

  int max_num_verts;
  std::vector<glm::vec3> verts;

  std::vector<std::unique_ptr<Geometry>> geometries;

  std::vector<std::unique_ptr<Light>> lights;

  glm::vec3 Trace(const Ray &ray, int depth) const;
};