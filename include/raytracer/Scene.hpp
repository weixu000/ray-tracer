#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "raytracer/Primitive.hpp"
#include "raytracer/Ray.hpp"
#include "raytracer/lights/Light.hpp"

class Scene {
public:
  std::string output_file;

  int max_num_verts;
  std::vector<glm::vec3> verts;

  std::vector<Primitive> primitives;

  std::vector<std::unique_ptr<Light>> lights;

  std::optional<RayHit> Trace(const Ray &ray) const;
};