#include "integrator.hpp"

#include <chrono>
#include <iostream>
#include <numeric>

#ifdef MULTITHREADING
#include "../thread_pool.hpp"
#endif

using namespace std;
using namespace glm;

Integrator::Integrator(Scene scene, Camera camera, float gamma)
    : camera_(move(camera)),
      gamma_(gamma),
      materials_(move(scene.materials)),
      lights_(move(scene.lights)),
      kernel_(move(scene.triangles), move(scene.triangle_materials),
              move(scene.sphere_world_transforms),
              move(scene.sphere_normal_transforms),
              move(scene.sphere_materials)) {}

Image Integrator::Render() const {
  using namespace glm;
  using namespace chrono;

  const auto w = camera_.width_;
  const auto h = camera_.height_;

  Image output(w, h);
  cout << "Rendering..." << endl;
  const auto start = steady_clock::now();
#ifdef MULTITHREADING
  {
    ThreadPool pool;

    for (int i = 0; i < h; ++i) {
      for (int j = 0; j < w; ++j) {
        pool.Add([this, &output, i, j]() {
          const auto color = pow(ShadePixel({i, j}), vec3{1 / gamma_});
          if (any(isnan(color))) cerr << "NaN Pixel: " << i << "," << j << endl;
          if (any(isinf(color))) cerr << "Inf Pixel: " << i << "," << j << endl;
          output.At(i, j) = u8vec3(min(color, vec3{1.f}) * 255.0f);
        });
      }
    }
  }
#else
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      const auto color = pow(ShadePixel({i, j}), vec3{1 / gamma_});
      if (any(isnan(color))) cerr << "NaN Pixel:" << i << "," << j << endl;
      if (any(isinf(color))) cerr << "Inf Pixel:" << i << "," << j << endl;
      output.At(i, j) = u8vec3(min(color, vec3{1.f}) * 255.0f);
    }
  }
#endif
  const auto end = steady_clock::now();
  cout << "Rendered in "
       << duration_cast<milliseconds>(end - start).count() / 1000.f << "s"
       << endl;

  return output;
}

const vector<RayHit>& Integrator::TraceShapesAll(const Ray& ray, float tnear,
                                                 float tfar) const {
  using namespace glm;
  thread_local vector<RayHit> hits;
  hits.clear();

  while (tnear < tfar) {
    if (const auto hit = TraceShapes(ray, tnear, tfar)) {
      hits.emplace_back(*hit);
      tnear = hit->t + 0.0001f;
    } else
      break;
  }
  return hits;
}