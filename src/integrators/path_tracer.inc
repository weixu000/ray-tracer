#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>

#include "../samplers/sampling.hpp"

template <bool russian_roulette, bool mis>
glm::vec3 PathTracer<russian_roulette, mis>::ShadePixel(
    const glm::vec2 &pixel) const {
  using namespace glm;

  auto radiance = vec3(0.f);
  for (int i = 0; i < num_pixel_sample_; ++i) {
    const auto ray = camera_.GenerateEyeRay(pixel + vec2{Random(), Random()});
    radiance += Trace(
        ray, [&](const Emission &emission) { return emission.L_e; },
        [&](const RayHit &hit) {
          const auto x = ray(hit.t), n = hit.n;
          const auto w_o = -ray.direction;
          if constexpr (russian_roulette) {
            return LightDirect(x, n, w_o, hit.mat->GetBSDF(n)) +
                   LightIndirect(x, n, w_o, hit.mat->GetBSDF(n), vec3(1.f));
          } else {
            return LightDirect(x, n, w_o, hit.mat->GetBSDF(n)) +
                   LightIndirect(x, n, w_o, hit.mat->GetBSDF(n),
                                 max_depth_ - 1);
          }
        });
  }
  return radiance / float(num_pixel_sample_);
}

template <bool russian_roulette, bool mis>
glm::vec3 PathTracer<russian_roulette, mis>::LightIndirect(
    const glm::vec3 &x, const glm::vec3 &n, const glm::vec3 &w_o,
    const BSDF *bsdf,
    std::conditional_t<russian_roulette, const glm::vec3 &, int>
        throughput_or_depth) const {
  using namespace glm;

  float q;
  if constexpr (russian_roulette) {
    q = 1 - min(1.f, compMax(throughput_or_depth));
    if (Random() <= q) {
      return vec3(0.f);
    }
  } else {
    if (throughput_or_depth <= 0) {
      return vec3(0.f);
    }
  }

  const auto w_i = bsdf->Sample(w_o);
  if (length(w_i) == 0) return vec3{0.f};
  const auto pdf = bsdf->Pdf(w_o, w_i);
  if (pdf == 0) return vec3{0.f};
  const auto ray_i = Ray{x + sign(dot(w_i, n)) * n * SHADOW_EPSILON, w_i};

  return Trace(
      ray_i, [&](const Emission &emission) { return vec3(0.f); },
      [&](const RayHit &hit) {
        const auto x_i = ray_i(hit.t), n_i = hit.n;
        if constexpr (russian_roulette) {
          const auto throughput_RR = bsdf->Brdf(w_i, w_o) * abs(dot(n, w_i)) /
                                     pdf * throughput_or_depth / (1 - q);
          return LightDirect(x_i, n_i, -w_i, hit.mat->GetBSDF(n_i)) *
                     throughput_RR +
                 LightIndirect(x_i, n_i, -w_i, hit.mat->GetBSDF(n_i),
                               throughput_RR);
        } else {
          const auto L = LightDirect(x_i, n_i, -w_i, hit.mat->GetBSDF(n_i)) +
                         LightIndirect(x_i, n_i, -w_i, hit.mat->GetBSDF(n_i),
                                       throughput_or_depth - 1);
          return L * bsdf->Brdf(w_i, w_o) * abs(dot(n, w_i)) / pdf;
        }
      });
}

template <bool russian_roulette, bool mis>
template <bool T>
std::enable_if_t<T, float> PathTracer<russian_roulette, mis>::PdfLight(
    const glm::vec3 &x, const glm::vec3 &n, const glm::vec3 &w_i) const {
  using namespace glm;

  const auto ray = Ray{x + sign(dot(w_i, n)) * n * SHADOW_EPSILON, w_i};
  auto pdf = 0.f;
  for (const auto &light : lights_) {
    if (const auto hit = light->Hit(ray)) {
      pdf += length2(ray(hit->t) - x) / (hit->jacobian * abs(dot(w_i, hit->n)));
    }
  }
  return pdf / lights_.size();
}

template <bool russian_roulette, bool mis>
template <bool brdf, bool T>
std::enable_if_t<T, glm::vec3> PathTracer<russian_roulette, mis>::MISSample(
    const glm::vec3 &x, const glm::vec3 &n, const glm::vec3 &w_i,
    const glm::vec3 &w_o, const BSDF *bsdf) const {
  using namespace glm;

  if (length(w_i) == 0) return vec3{0.f};
  const auto ray_i = Ray{x + sign(dot(w_i, n)) * n * SHADOW_EPSILON, w_i};
  if (const auto light_hit = TraceLights(ray_i)) {
    if (const auto shadow_hit = TraceShapes(ray_i);
        !shadow_hit || shadow_hit->t > light_hit->t) {
      const auto pdf = brdf ? bsdf->Pdf(w_o, w_i) : PdfLight(x, n, w_i);
      const auto other_pdf = !brdf ? bsdf->Pdf(w_o, w_i) : PdfLight(x, n, w_i);
      if (pdf == 0) return vec3{0.f};
      const auto w = float(pow(pdf, 2) / (pow(pdf, 2) + pow(other_pdf, 2)));
      const auto f = light_hit->L_e * bsdf->Brdf(w_i, w_o) * abs(dot(n, w_i));
      return f * w / pdf;
    }
  }
  return vec3{0.f};
}

template <bool russian_roulette, bool mis>
glm::vec3 PathTracer<russian_roulette, mis>::LightDirect(
    const glm::vec3 &x, const glm::vec3 &n, const glm::vec3 &w_o,
    const BSDF *bsdf) const {
  using namespace glm;

  auto radiance = vec3{0.f};

  if constexpr (mis) {
    radiance += MISSample<true>(x, n, bsdf->Sample(w_o), w_o, bsdf);
    for (const auto &light : lights_) {
      const auto w_i = normalize(light->Sample() - x);
      radiance +=
          MISSample<false>(x, n, w_i, w_o, bsdf) / float(lights_.size());
    }
  } else {
    for (const auto &light : lights_) {
      const auto sample = light->Sample();
      const auto w_i = normalize(sample - x);
      const auto ray_i = Ray{x + sign(dot(w_i, n)) * n * SHADOW_EPSILON, w_i};
      if (const auto light_hit = TraceLights(ray_i);
          light_hit && light_hit->light == light.get()) {
        if (const auto shadow_hit = TraceShapes(ray_i);
            !shadow_hit || shadow_hit->t > light_hit->t) {
          const auto n_l = light_hit->n;
          const auto G =
              abs(dot(n, w_i)) * max(0.f, dot(w_i, n_l)) / length2(sample - x);
          radiance +=
              light_hit->L_e * light_hit->jacobian * bsdf->Brdf(w_i, w_o) * G;
        }
      }
    }
  }

  return radiance;
}
