#pragma once
#include <array>
#include <glm/glm.hpp>
#include <optional>
#include <vector>

#include "geometry.hpp"
#include "ray.hpp"

typedef struct RTCDeviceTy* RTCDevice;
typedef struct RTCSceneTy* RTCScene;

class Kernel {
 public:
  Kernel(const std::vector<Mesh>& meshes, const std::vector<Sphere>& spheres);

  std::optional<RayHit> TraceShapes(const Ray& ray, float tnear = 0.f,
                                    float tfar = FLT_MAX) const;

  void TraceShapesAll(std::vector<RayHit>& out, const Ray& ray,
                      float tnear = 0.f, float tfar = FLT_MAX) const;

 private:
  void LoadEmbreeTriangles(const std::vector<Mesh>& meshes);

  void loadEmbreeSpheres(const std::vector<Sphere>& spheres);

  std::vector<MaterialRef> triangle_materials_;
  std::vector<MaterialRef> sphere_materials_;

  std::vector<glm::mat3> sphere_normal_transforms_;

  RTCDevice embree_device_;
  RTCScene embree_scene_;
};
