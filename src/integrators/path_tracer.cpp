#include "path_tracer.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>

using namespace std;
using namespace glm;

vec3 PathTracer::ShadePixel(const vec2 &pixel) const {
  auto radiance = vec3(0.f);
  for (int i = 0; i < num_pixel_sample_; ++i) {
    const auto ray = camera_.GenerateEyeRay(pixel + vec2{Random(), Random()});
    radiance += Trace(
        ray, [&](const Emission &emission) { return emission.L_e; },
        [&](const RayHit &hit) {
          return BounceOut(
              hit, -ray.direction,
              [&](const vec3 &x, const vec3 &n, const auto &bounce) {
                return LightDirect(x, n, bounce) +
                       LightIndirect(x, n, bounce, vec3{1.f}, max_depth_ - 1);
              });
        });
  }
  return radiance / float(num_pixel_sample_);
}

template <typename Bounce>
vec3 PathTracer::LightIndirect(const vec3 &x, const vec3 &n,
                               const Bounce &bounce, const vec3 &throughput,
                               int depth) const {
  float q;
  if (russian_roulette_) {
    q = 1 - glm::min(1.f, compMax(throughput));
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
        auto tp = bounce.Value(w_i) * abs(dot(n, w_i)) / pdf * throughput;
        if (russian_roulette_) {
          tp /= (1 - q);
        }
        return BounceOut(hit, -w_i,
                         [&](const vec3 &x, const vec3 &n, const auto &b_i) {
                           return LightDirect(x, n, b_i) * tp +
                                  LightIndirect(x, n, b_i, tp, depth - 1);
                         });
      });
}

template <typename Bounce>
vec3 PathTracer::LightDirect(const vec3 &x, const vec3 &n,
                             const Bounce &bounce) const {
  auto radiance = vec3{0.f};

  if (mis_) {
    {
      const auto w_i = bounce.Sample();
      radiance +=
          MISSample(x, n, w_i, bounce, bounce.Pdf(w_i), PdfLight(x, n, w_i));
    }
    for (const auto &light : lights_) {
      const auto w_i = normalize(light->Sample() - x);
      radiance +=
          MISSample(x, n, w_i, bounce, PdfLight(x, n, w_i), bounce.Pdf(w_i)) /
          float(lights_.size());
    }
  } else {
    for (const auto &light : lights_) {
      const auto sample = light->Sample();
      const auto w_i = normalize(sample - x);
      const auto ray_i = Ray{x + sign(dot(w_i, n)) * n * SHADOW_EPSILON, w_i};
      if (const auto light_hit = TraceLights(ray_i);
          light_hit && light_hit->light == light.get()) {
        if (const auto shadow_hit = kernel_.TraceShapes(ray_i);
            !shadow_hit || shadow_hit->t > light_hit->t) {
          const auto n_l = light_hit->n;
          const auto G = abs(dot(n, w_i)) * glm::max(0.f, dot(w_i, n_l)) /
                         length2(sample - x);
          radiance +=
              light_hit->L_e * light_hit->jacobian * bounce.Value(w_i) * G;
        }
      }
    }
  }

  return radiance;
}

float PathTracer::PdfLight(const vec3 &x, const vec3 &n,
                           const vec3 &w_i) const {
  const auto ray = Ray{x + sign(dot(w_i, n)) * n * SHADOW_EPSILON, w_i};
  auto pdf = 0.f;
  for (const auto &light : lights_) {
    if (const auto hit = light->Hit(ray)) {
      pdf += length2(ray(hit->t) - x) / (hit->jacobian * abs(dot(w_i, hit->n)));
    }
  }
  return pdf / lights_.size();
}

template <typename Bounce>
vec3 PathTracer::MISSample(const vec3 &x, const vec3 &n, const vec3 &w_i,
                           const Bounce &bounce, float pdf,
                           float other_pdf) const {
  if (length(w_i) == 0) return vec3{0.f};
  const auto ray_i = Ray{x + sign(dot(w_i, n)) * n * SHADOW_EPSILON, w_i};
  if (const auto light_hit = TraceLights(ray_i)) {
    if (const auto shadow_hit = kernel_.TraceShapes(ray_i);
        !shadow_hit || shadow_hit->t > light_hit->t) {
      if (pdf == 0) return vec3{0.f};
      const auto w = float(pow(pdf, 2) / (pow(pdf, 2) + pow(other_pdf, 2)));
      const auto f = light_hit->L_e * bounce.Value(w_i) * abs(dot(n, w_i));
      return f * w / pdf;
    }
  }
  return vec3{0.f};
}

namespace {
template <typename BSDF>
struct BSDFBounce {
  vec3 Value(const vec3 &w_i) const { return bsdf.Value(w_i, w_o); }

  vec3 Sample() const { return bsdf.Sample(w_o); }

  float Pdf(const vec3 &w_i) const { return bsdf.Pdf(w_o, w_i); }

  BSDF bsdf;
  vec3 w_o;
};

template <typename T, typename U>
BSDFBounce(T, U) -> BSDFBounce<T>;

struct BSSRDFBounce {
  vec3 Value(const vec3 &w_i) const {
    return bssrdf.Value(p_i, n_i, w_i, p_o, n_o, w_o) / pdf_x_i;
  }

  vec3 Sample() const { return bssrdf.SampleWi(n_i); }

  float Pdf(const vec3 &w_i) const { return bssrdf.PdfWi(n_i, w_i); }

  BSSRDF bssrdf;
  vec3 p_i{}, n_i{}, p_o{}, n_o{}, w_o{};
  float pdf_x_i{};
};

}  // namespace

template <typename F>
glm::vec3 PathTracer::BounceOut(const RayHit &hit, const vec3 &w_o,
                                F &&f) const {
  return visit(
      [&](const auto &m) {
        const auto bxdf = m.GetBxDF(hit.n);
        if constexpr (std::is_same_v<std::decay_t<decltype(bxdf)>, BSSRDF>) {
          if (const auto sample =
                  bxdf.SamplePi(hit.p, hit.n, [this, &hit](auto &&... args) {
                    kernel_.TraceGeometry(
                        hit.id, std::forward<decltype(args)>(args)...);
                  })) {
            const auto [hit_i, pdf_x_i] = *sample;

            BSSRDFBounce bounce;
            bounce.bssrdf = move(bxdf);
            bounce.p_i = hit_i.p;
            bounce.n_i = hit_i.n;
            bounce.p_o = hit.p;
            bounce.n_o = hit.n;
            bounce.w_o = w_o;
            bounce.pdf_x_i = pdf_x_i;
            return f(hit_i.p, hit_i.n, bounce);
          } else
            return vec3{0.f};
        } else {
          return f(hit.p, hit.n, BSDFBounce{bxdf, w_o});
        }
      },
      materials_[hit.mat]);
}
