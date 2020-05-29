#include "kernel.hpp"

#include <embree3/rtcore.h>

#include <cstring>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <numeric>
#include <utility>

using namespace std;
using namespace glm;

namespace {
void EmbreeErrorFunction(void*, RTCError error, const char* str) {
  cerr << "Embree error (" << error << "): " << str << endl;
}
}  // namespace

Kernel::Kernel(const vector<Mesh>& meshes, const vector<Sphere>& spheres) {
  embree_device_ = rtcNewDevice(nullptr);
  if (!embree_device_)
    throw runtime_error("Could not initialize Embree device.");

  rtcSetDeviceErrorFunction(embree_device_, EmbreeErrorFunction, nullptr);

  embree_scene_ = rtcNewScene(embree_device_);
  LoadEmbreeTriangles(meshes);
  loadEmbreeSpheres(spheres);
  rtcCommitScene(embree_scene_);
}

void Kernel::LoadEmbreeTriangles(const vector<Mesh>& meshes) {
  for (const auto& m : meshes) {
    auto triangles = rtcNewGeometry(embree_device_, RTC_GEOMETRY_TYPE_TRIANGLE);

    auto vertex_buffer = reinterpret_cast<vec3*>(rtcSetNewGeometryBuffer(
        triangles, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(vec3),
        m.vertices.size()));
    memcpy(vertex_buffer, m.vertices.data(), m.vertices.size() * sizeof(vec3));

    auto index_buffer = reinterpret_cast<uvec3*>(rtcSetNewGeometryBuffer(
        triangles, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(uvec3),
        m.indices.size()));
    memcpy(index_buffer, m.indices.data(), m.indices.size() * sizeof(uvec3));

    rtcCommitGeometry(triangles);
    rtcAttachGeometry(embree_scene_, triangles);
    rtcReleaseGeometry(triangles);

    triangle_materials_.emplace_back(m.material);
  }
}

void Kernel::loadEmbreeSpheres(const vector<Sphere>& spheres) {
  auto sphere_scene = rtcNewScene(embree_device_);

  RTCGeometry sphere =
      rtcNewGeometry(embree_device_, RTC_GEOMETRY_TYPE_SPHERE_POINT);

  auto sphere_point = reinterpret_cast<vec4*>(rtcSetNewGeometryBuffer(
      sphere, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT4, sizeof(vec4), 1));
  *sphere_point = vec4(0.0f, 0.0f, 0.0f, 1.0f);

  rtcCommitGeometry(sphere);
  rtcAttachGeometry(sphere_scene, sphere);
  rtcReleaseGeometry(sphere);
  rtcCommitScene(sphere_scene);

  for (const auto& s : spheres) {
    auto instance = rtcNewGeometry(embree_device_, RTC_GEOMETRY_TYPE_INSTANCE);
    rtcSetGeometryInstancedScene(instance, sphere_scene);
    rtcSetGeometryTimeStepCount(instance, 1);
    rtcSetGeometryTransform(instance, 0, RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR,
                            value_ptr(s.transform));
    rtcCommitGeometry(instance);
    rtcAttachGeometry(embree_scene_, instance);
    rtcReleaseGeometry(instance);

    sphere_normal_transforms_.emplace_back(
        mat3(inverse(transpose(mat3(s.transform)))));
    sphere_materials_.emplace_back(s.material);
  }

  rtcReleaseScene(sphere_scene);
}

optional<RayHit> Kernel::TraceShapes(const Ray& ray, float tnear,
                                     float tfar) const {
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);

  RTCRayHit rayHit;
  rayHit.ray.org_x = ray.origin.x;
  rayHit.ray.org_y = ray.origin.y;
  rayHit.ray.org_z = ray.origin.z;
  rayHit.ray.dir_x = ray.direction.x;
  rayHit.ray.dir_y = ray.direction.y;
  rayHit.ray.dir_z = ray.direction.z;
  rayHit.ray.tnear = tnear;
  rayHit.ray.tfar = tfar;
  rayHit.ray.mask = 0;
  rayHit.ray.flags = 0;
  rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
  rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

  rtcIntersect1(embree_scene_, &context, &rayHit);

  if (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
    RayHit hit;
    hit.t = rayHit.ray.tfar;
    hit.p = ray(hit.t);
    const auto hitNormal =
        normalize(vec3(rayHit.hit.Ng_x, rayHit.hit.Ng_y, rayHit.hit.Ng_z));
    if (rayHit.hit.instID[0] == RTC_INVALID_GEOMETRY_ID) {
      hit.n = hitNormal;
      hit.mat = triangle_materials_[rayHit.hit.geomID];
    } else {
      const auto sphereIndex =
          rayHit.hit.instID[0] - triangle_materials_.size();
      hit.n = normalize(sphere_normal_transforms_[sphereIndex] * hitNormal);
      hit.mat = sphere_materials_[sphereIndex];
    }
    return hit;
  } else
    return nullopt;
}

void Kernel::TraceShapesAll(vector<RayHit>& out, const Ray& ray, float tnear,
                            float tfar) const {
  out.clear();
  while (tnear < tfar) {
    if (const auto hit = TraceShapes(ray, tnear, tfar)) {
      out.emplace_back(*hit);
      tnear = hit->t + 0.0001f;
    } else
      break;
  }
}
