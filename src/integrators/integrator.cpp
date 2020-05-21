#include "integrator.hpp"

#include <embree3/rtcore.h>

#include <chrono>
#include <cstring>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <numeric>

#ifdef MULTITHREADING
#include "../thread_pool.hpp"
#endif

namespace {
void EmbreeErrorFunction(void*, RTCError error, const char* str) {
  std::cerr << "Embree error (" << error << "): " << str << std::endl;
}

void LoadEmbreeTriangles(
    RTCDevice device, RTCScene scene,
    const std::vector<std::array<glm::vec3, 3>>& vertices) {
  auto triangles = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

  auto vertex_buffer = reinterpret_cast<glm::vec3*>(rtcSetNewGeometryBuffer(
      triangles, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
      sizeof(glm::vec3), vertices.size() * 3));
  std::memcpy(vertex_buffer, vertices.data(),
              vertices.size() * sizeof(glm::vec3) * 3);

  auto index_buffer = reinterpret_cast<glm::uint*>(rtcSetNewGeometryBuffer(
      triangles, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(glm::uvec3),
      vertices.size() * 3));
  std::iota(index_buffer, index_buffer + vertices.size() * 3, 0U);

  rtcCommitGeometry(triangles);
  rtcAttachGeometry(scene, triangles);
  rtcReleaseGeometry(triangles);
}

void loadEmbreeSpheres(RTCDevice device, RTCScene scene,
                       const std::vector<glm::mat4>& transforms) {
  auto sphere_scene = rtcNewScene(device);

  RTCGeometry sphere = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_SPHERE_POINT);

  auto sphere_point = reinterpret_cast<glm::vec4*>(
      rtcSetNewGeometryBuffer(sphere, RTC_BUFFER_TYPE_VERTEX, 0,
                              RTC_FORMAT_FLOAT4, sizeof(glm::vec4), 1));
  *sphere_point = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

  rtcCommitGeometry(sphere);
  rtcAttachGeometry(sphere_scene, sphere);
  rtcReleaseGeometry(sphere);
  rtcCommitScene(sphere_scene);

  for (const auto& transform : transforms) {
    auto instance = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_INSTANCE);
    rtcSetGeometryInstancedScene(instance, sphere_scene);
    rtcSetGeometryTimeStepCount(instance, 1);
    rtcSetGeometryTransform(instance, 0, RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR,
                            glm::value_ptr(transform));
    rtcCommitGeometry(instance);
    rtcAttachGeometry(scene, instance);
    rtcReleaseGeometry(instance);
  }

  rtcReleaseScene(sphere_scene);
}

}  // namespace

Integrator::Integrator(Scene scene, Camera camera, float gamma)
    : scene_(std::move(scene)), camera_(std::move(camera)), gamma_(gamma) {
  embree_device_ = rtcNewDevice(nullptr);
  if (!embree_device_)
    throw std::runtime_error("Could not initialize Embree device.");

  rtcSetDeviceErrorFunction(embree_device_, EmbreeErrorFunction, nullptr);

  embree_scene_ = rtcNewScene(embree_device_);
  LoadEmbreeTriangles(embree_device_, embree_scene_, scene_.triangles);
  loadEmbreeSpheres(embree_device_, embree_scene_,
                    scene_.sphere_world_transforms);
  rtcCommitScene(embree_scene_);
}

Integrator::~Integrator() {
  rtcReleaseScene(embree_scene_);
  rtcReleaseDevice(embree_device_);
}

Image Integrator::Render() const {
  using namespace glm;
  using namespace std::chrono;

  const auto w = camera_.width_;
  const auto h = camera_.height_;

  Image output(w, h);
  std::cout << "Rendering..." << std::endl;
  const auto start = steady_clock::now();
#ifdef MULTITHREADING
  {
    ThreadPool pool;

    for (int i = 0; i < h; ++i) {
      for (int j = 0; j < w; ++j) {
        pool.Add([this, &output, i, j]() {
          const auto color = pow(ShadePixel({i, j}), vec3{1 / gamma_});
          if (any(isnan(color)))
            std::cerr << "NaN Pixel: " << i << "," << j << std::endl;
          if (any(isinf(color)))
            std::cerr << "Inf Pixel: " << i << "," << j << std::endl;
          output.At(i, j) = u8vec3(min(color, vec3{1.f}) * 255.0f);
        });
      }
    }
  }
#else
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      const auto color = pow(ShadePixel({i, j}), vec3{1 / gamma_});
      if (any(isnan(color)))
        std::cerr << "NaN Pixel:" << i << "," << j << std::endl;
      if (any(isinf(color)))
        std::cerr << "Inf Pixel:" << i << "," << j << std::endl;
      output.At(i, j) = u8vec3(min(color, vec3{1.f}) * 255.0f);
    }
  }
#endif
  const auto end = steady_clock::now();
  std::cout << "Rendered in "
            << duration_cast<milliseconds>(end - start).count() / 1000.f << "s"
            << std::endl;

  return output;
}

std::optional<RayHit> Integrator::TraceShapes(const Ray& ray) const {
  using namespace glm;

  RTCIntersectContext context;
  rtcInitIntersectContext(&context);

  RTCRayHit rayHit;
  rayHit.ray.org_x = ray.origin.x;
  rayHit.ray.org_y = ray.origin.y;
  rayHit.ray.org_z = ray.origin.z;
  rayHit.ray.dir_x = ray.direction.x;
  rayHit.ray.dir_y = ray.direction.y;
  rayHit.ray.dir_z = ray.direction.z;
  rayHit.ray.tnear = 0.0001f;
  rayHit.ray.tfar = std::numeric_limits<float>::infinity();
  rayHit.ray.mask = 0;
  rayHit.ray.flags = 0;
  rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
  rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

  rtcIntersect1(embree_scene_, &context, &rayHit);

  if (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
    RayHit hit;
    hit.t = rayHit.ray.tfar;
    const auto hitNormal =
        normalize(vec3(rayHit.hit.Ng_x, rayHit.hit.Ng_y, rayHit.hit.Ng_z));
    if (rayHit.hit.instID[0] == RTC_INVALID_GEOMETRY_ID) {
      hit.n = hitNormal;
      hit.mat = scene_.triangle_materials[rayHit.hit.primID];
    } else {
      const auto sphereIndex = rayHit.hit.instID[0] - 1;
      hit.n =
          normalize(scene_.sphere_normal_transforms[sphereIndex] * hitNormal);
      hit.mat = scene_.sphere_materials[sphereIndex];
    }
    return hit;
  } else
    return std::nullopt;
}
