#include "integrator.hpp"

#include <chrono>
#include <iostream>

#ifdef MULTITHREADING
#include "../thread_pool.hpp"
#endif

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
