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
  Kernel(std::vector<Geometry> geometries);

  std::optional<RayHit> TraceShapes(const Ray& ray, float tnear = 0.f,
                                    float tfar = FLT_MAX) const;

  void TraceShapesAll(std::vector<RayHit>& out, const Ray& ray,
                      float tnear = 0.f, float tfar = FLT_MAX) const;

 private:
  void LoadEmbreeMeshes();

  void loadEmbreeSpheres();

  std::vector<Geometry> geometries_;

  RTCDevice embree_device_;
  RTCScene embree_scene_;
};
