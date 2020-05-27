#include "path_tracer.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>

#include "../samplers/sampling.hpp"

template <typename BSDF>
struct BSDFBounce {
  BSDF bsdf;
  const glm::vec3 w_o;

  glm::vec3 Value(const glm::vec3 &w_i) const { return bsdf.Value(w_i, w_o); }

  glm::vec3 Sample() const { return bsdf.Sample(w_o); }

  float Pdf(const glm::vec3 &w_i) const { return bsdf.Pdf(w_o, w_i); }
};

template <typename T, typename U>
BSDFBounce(T, U) -> BSDFBounce<T>;  // this is a deduction guide

glm::vec3 PathTracer::ShadePixel(const glm::vec2 &pixel) const {
  using namespace glm;

  auto radiance = vec3(0.f);
  for (int i = 0; i < num_pixel_sample_; ++i) {
    const auto ray = camera_.GenerateEyeRay(pixel + vec2{Random(), Random()});
    radiance += Trace(
        ray, [&](const Emission &emission) { return emission.L_e; },
        [&](const RayHit &hit) {
          const auto x = ray(hit.t), n = hit.n;
          const auto w_o = -ray.direction;
          return std::visit(
              [&](const auto &m) {
                const BSDFBounce bounce{m.GetBSDF(n), w_o};
                return LightDirect(x, n, bounce) +
                       LightIndirect(x, n, bounce, vec3(1.f), max_depth_ - 1);
              },
              materials_[hit.mat]);
        });
  }
  return radiance / float(num_pixel_sample_);
}

template <typename Bounce>
glm::vec3 PathTracer::LightIndirect(const glm::vec3 &x, const glm::vec3 &n,
                                    const Bounce &bounce,
                                    const glm::vec3 &throughput,
                                    int depth) const {
  using namespace glm;

  float q;
  if (russian_roulette_) {
    q = 1 - min(1.f, compMax(throughput));
    if (Random() <= q) {
      return vec3(0.f);
    }
  } else {
    if (depth <= 0) {
      return vec3(0.f);
    }
  }

  const auto w_i = bounce.Sample();
  if (length(w_i) == 0) return vec3{0.f};
  const auto pdf = bounce.Pdf(w_i);
  if (pdf == 0) return vec3{0.f};
  const auto ray_i = Ray{x + sign(dot(w_i, n)) * n * SHADOW_EPSILON, w_i};

  return Trace(
      ray_i, [&](const Emission &emission) { return vec3(0.f); },
      [&](const RayHit &hit) {
        const auto x_i = ray_i(hit.t), n_i = hit.n;
        return std::visit(
            [&](const auto &m) {
              const BSDFBounce b_i{m.GetBSDF(n_i), -w_i};
              if (russian_roulette_) {
                const auto throughput_RR = bounce.Value(w_i) *
                                           abs(dot(n, w_i)) / pdf * throughput /
                                           (1 - q);
                return LightDirect(x_i, n_i, b_i) * throughput_RR +
                       LightIndirect(x_i, n_i, b_i, throughput_RR, depth - 1);
              } else {
                const auto L =
                    LightDirect(x_i, n_i, b_i) +
                    LightIndirect(x_i, n_i, b_i, throughput, depth - 1);
                return L * bounce.Value(w_i) * abs(dot(n, w_i)) / pdf;
              }
            },
            materials_[hit.mat]);
      });
}

float PathTracer::PdfLight(const glm::vec3 &x, const glm::vec3 &n,
                           const glm::vec3 &w_i) const {
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

template <bool brdf, typename Bounce>
glm::vec3 PathTracer::MISSample(const glm::vec3 &x, const glm::vec3 &n,
                                const glm::vec3 &w_i,
                                const Bounce &bounce) const {
  using namespace glm;

  if (length(w_i) == 0) return vec3{0.f};
  const auto ray_i = Ray{x + sign(dot(w_i, n)) * n * SHADOW_EPSILON, w_i};
  if (const auto light_hit = TraceLights(ray_i)) {
    if (const auto shadow_hit = TraceShapes(ray_i);
        !shadow_hit || shadow_hit->t > light_hit->t) {
      const auto pdf = brdf ? bounce.Pdf(w_i) : PdfLight(x, n, w_i);
      const auto other_pdf = !brdf ? bounce.Pdf(w_i) : PdfLight(x, n, w_i);
      if (pdf == 0) return vec3{0.f};
      const auto w = float(pow(pdf, 2) / (pow(pdf, 2) + pow(other_pdf, 2)));
      const auto f = light_hit->L_e * bounce.Value(w_i) * abs(dot(n, w_i));
      return f * w / pdf;
    }
  }
  return vec3{0.f};
}

template <typename Bounce>
glm::vec3 PathTracer::LightDirect(const glm::vec3 &x, const glm::vec3 &n,
                                  const Bounce &bounce) const {
  using namespace glm;

  auto radiance = vec3{0.f};

  if (mis_) {
    radiance += MISSample<true>(x, n, bounce.Sample(), bounce);
    for (const auto &light : lights_) {
      const auto w_i = normalize(light->Sample() - x);
      radiance += MISSample<false>(x, n, w_i, bounce) / float(lights_.size());
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
              light_hit->L_e * light_hit->jacobian * bounce.Value(w_i) * G;
        }
      }
    }
  }

  return radiance;
}
