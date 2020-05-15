#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>

#include "../samplers/sampling.hpp"

template <bool russian_roulette, Sampling sampling, bool mis>
glm::vec3 PathTracer<russian_roulette, sampling, mis>::Sample(
    const Ray &ray) const {
  using namespace glm;

  return Trace(
      ray, [&](const LightEmission &emission) { return emission.L_e; },
      [&](const RayHit &hit) {
        const auto x = ray(hit.t), n = hit.n;
        const auto w_o = -ray.direction;
        if constexpr (russian_roulette) {
          return LightDirect(x, n, w_o, hit.mat) +
                 LightIndirect(x, n, w_o, hit.mat, vec3(1.f));
        } else {
          return LightDirect(x, n, w_o, hit.mat) +
                 LightIndirect(x, n, w_o, hit.mat, max_depth_ - 1);
        }
      });
}

template <bool russian_roulette, Sampling sampling, bool mis>
glm::vec3 PathTracer<russian_roulette, sampling, mis>::SampleBrdf(
    const MaterialRef &mat, const glm::vec3 &n, const glm::vec3 &w_o) const {
  switch (sampling) {
    case Sampling::Hemisphere: {
      static HemisphereSampler sampler;
      return sampler.Sample(n);
    }
    case Sampling::Cosine: {
      static CosineSampler sampler;
      return sampler.Sample(n);
    }
    case Sampling::BRDF:
      return scene_.Sample(mat, n, w_o);
  }
}

template <bool russian_roulette, Sampling sampling, bool mis>
float PathTracer<russian_roulette, sampling, mis>::PdfBrdf(
    const MaterialRef &mat, const glm::vec3 &n, const glm::vec3 &w_i,
    const glm::vec3 &w_o) const {
  switch (sampling) {
    case Sampling::Hemisphere: {
      static HemisphereSampler sampler;
      return sampler.Pdf();
    }
    case Sampling::Cosine: {
      static CosineSampler sampler;
      return sampler.Pdf(n, w_i);
    }
    case Sampling::BRDF:
      return scene_.Pdf(mat, n, w_i, w_o);
  }
}

template <bool russian_roulette, Sampling sampling, bool mis>
glm::vec3 PathTracer<russian_roulette, sampling, mis>::LightIndirect(
    const glm::vec3 &x, const glm::vec3 &n, const glm::vec3 &w_o,
    const MaterialRef &mat,
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

  const auto w_i = SampleBrdf(mat, n, w_o);
  const auto pdf = PdfBrdf(mat, n, w_i, w_o);
  if (pdf == 0) return vec3{0.f};
  const auto ray_i = Ray{x + w_i * SHADOW_EPSILON, w_i};

  return Trace(
      ray_i, [&](const LightEmission &emission) { return vec3(0.f); },
      [&](const RayHit &hit) {
        const auto x_i = ray_i(hit.t), n_i = hit.n;
        if constexpr (russian_roulette) {
          const auto throughput_RR = scene_.Brdf(mat, n, w_i, w_o) *
                                     abs(dot(n, w_i)) / pdf *
                                     throughput_or_depth / (1 - q);
          return LightDirect(x_i, n_i, -w_i, hit.mat) * throughput_RR +
                 LightIndirect(x_i, n_i, -w_i, hit.mat, throughput_RR);
        } else {
          const auto L =
              LightDirect(x_i, n_i, -w_i, hit.mat) +
              LightIndirect(x_i, n_i, -w_i, hit.mat, throughput_or_depth - 1);
          return L * scene_.Brdf(mat, n, w_i, w_o) * abs(dot(n, w_i)) / pdf;
        }
      });
}

template <bool russian_roulette, Sampling sampling, bool mis>
float PathTracer<russian_roulette, sampling, mis>::PdfLight(
    const glm::vec3 &x, const glm::vec3 &w_i) const {
  using namespace glm;

  const auto ray = Ray{x + w_i * SHADOW_EPSILON, w_i};
  auto pdf = 0.f;
  for (const auto &light : scene_.lights) {
    if (const auto hit = light->Hit(ray)) {
      pdf += length2(ray(hit->t) - x) / (hit->jacobian * abs(dot(w_i, hit->n)));
    }
  }
  return pdf / scene_.lights.size();
}

template <bool russian_roulette, Sampling sampling, bool mis>
template <bool brdf, bool T>
std::enable_if_t<T, glm::vec3>
PathTracer<russian_roulette, sampling, mis>::MISSample(
    const glm::vec3 &x, const glm::vec3 &n, const glm::vec3 &w_i,
    const glm::vec3 &w_o, const MaterialRef &mat) const {
  using namespace glm;

  const auto ray_i = Ray{x + w_i * SHADOW_EPSILON, w_i};
  if (const auto light_hit = TraceLights(ray_i)) {
    if (const auto shadow_hit = TraceShapes(ray_i);
        !shadow_hit || shadow_hit->t > light_hit->t) {
      const auto pdf = brdf ? PdfBrdf(mat, n, w_i, w_o) : PdfLight(x, w_i);
      const auto other_pdf =
          !brdf ? PdfBrdf(mat, n, w_i, w_o) : PdfLight(x, w_i);
      if (pdf == 0) return vec3{0.f};
      const auto w = float(pow(pdf, 2) / (pow(pdf, 2) + pow(other_pdf, 2)));
      const auto f =
          light_hit->L_e * scene_.Brdf(mat, n, w_i, w_o) * abs(dot(n, w_i));
      return f * w / pdf;
    }
  }
  return vec3{0.f};
}

template <bool russian_roulette, Sampling sampling, bool mis>
glm::vec3 PathTracer<russian_roulette, sampling, mis>::LightDirect(
    const glm::vec3 &x, const glm::vec3 &n, const glm::vec3 &w_o,
    const MaterialRef &mat) const {
  using namespace glm;

  auto radiance = vec3{0.f};

  if constexpr (mis) {
    radiance += MISSample<true>(x, n, SampleBrdf(mat, n, w_o), w_o, mat);
    for (const auto &light : scene_.lights) {
      const auto w_i = normalize(light->Sample() - x);
      radiance +=
          MISSample<false>(x, n, w_i, w_o, mat) / float(scene_.lights.size());
    }
  } else {
    for (const auto &light : scene_.lights) {
      const auto sample = light->Sample();
      const auto w_i = normalize(sample - x);
      const auto ray_i = Ray{x + w_i * SHADOW_EPSILON, w_i};
      if (const auto light_hit = TraceLights(ray_i);
          light_hit && light_hit->light == light.get()) {
        if (const auto shadow_hit = TraceShapes(ray_i);
            !shadow_hit || shadow_hit->t > light_hit->t) {
          const auto n_l = light_hit->n;
          const auto G =
              abs(dot(n, w_i)) * max(0.f, dot(w_i, n_l)) / length2(sample - x);
          radiance += light_hit->L_e * light_hit->jacobian *
                      scene_.Brdf(mat, n, w_i, w_o) * G;
        }
      }
    }
  }

  return radiance;
}
