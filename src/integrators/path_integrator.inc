#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>

#include "../samplers/random.hpp"

template <bool russian_roulette, Sampling sampling, bool mis>
glm::vec3 PathIntegrator<russian_roulette, sampling, mis>::Sample(
    const Ray &ray) const {
  using namespace glm;

  return scene_.Trace(
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
glm::vec3 PathIntegrator<russian_roulette, sampling, mis>::SampleBrdf(
    const MaterialRef &material, const glm::vec3 &n,
    const glm::vec3 &w_o) const {
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
      return scene_.Sample(material, n, w_o);
  }
}

template <bool russian_roulette, Sampling sampling, bool mis>
float PathIntegrator<russian_roulette, sampling, mis>::PdfBrdf(
    const MaterialRef &material, const glm::vec3 &n, const glm::vec3 &w_i,
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
      return scene_.Pdf(material, n, w_i, w_o);
  }
}

template <bool russian_roulette, Sampling sampling, bool mis>
glm::vec3 PathIntegrator<russian_roulette, sampling, mis>::LightIndirect(
    const glm::vec3 &x, const glm::vec3 &n, const glm::vec3 &w_o,
    const MaterialRef &material,
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

  const auto w_i = SampleBrdf(material, n, w_o);
  const auto pdf = PdfBrdf(material, n, w_i, w_o);
  if (dot(w_i, n) <= 0 || pdf == 0) return vec3{0.f};
  const auto ray_i = Ray{x + w_i * SHADOW_EPSILON, w_i};

  return scene_.Trace(
      ray_i, [&](const LightEmission &emission) { return vec3(0.f); },
      [&](const RayHit &hit) {
        const auto x_i = ray_i(hit.t), n_i = hit.n;
        if constexpr (russian_roulette) {
          const auto throughput_RR = scene_.Brdf(material, n, w_i, w_o) *
                                     max(0.f, dot(n, w_i)) / pdf *
                                     throughput_or_depth / (1 - q);
          return LightDirect(x_i, n_i, -w_i, hit.mat) * throughput_RR +
                 LightIndirect(x_i, n_i, -w_i, hit.mat, throughput_RR);
        } else {
          const auto L =
              LightDirect(x_i, n_i, -w_i, hit.mat) +
              LightIndirect(x_i, n_i, -w_i, hit.mat, throughput_or_depth - 1);
          return L * scene_.Brdf(material, n, w_i, w_o) *
                 max(0.f, dot(n, w_i)) / pdf;
        }
      });
}

template <bool russian_roulette, Sampling sampling, bool mis>
float PathIntegrator<russian_roulette, sampling, mis>::PdfLight(
    const glm::vec3 &x, const glm::vec3 &w_i) const {
  using namespace glm;

  const auto ray = Ray{x + w_i * SHADOW_EPSILON, w_i};
  auto pdf = 0.f;
  for (const auto &light : scene_.lights) {
    if (const auto hit = light->Hit(ray)) {
      pdf += length2(ray(hit->distance) - x) /
             (hit->jacobian * abs(dot(w_i, normalize(hit->normal))));
    }
  }
  return pdf / scene_.lights.size();
}

template <bool russian_roulette, Sampling sampling, bool mis>
template <bool brdf, bool T>
std::enable_if_t<T, glm::vec3>
PathIntegrator<russian_roulette, sampling, mis>::MISSample(
    const glm::vec3 &x, const glm::vec3 &n, const glm::vec3 &w_i,
    const glm::vec3 &w_o, const MaterialRef &mat) const {
  using namespace glm;

  const auto ray_i = Ray{x + w_i * SHADOW_EPSILON, w_i};
  if (const auto light_hit = scene_.TraceLights(ray_i)) {
    if (const auto shadow_hit = scene_.TraceShapes(ray_i);
        !shadow_hit || shadow_hit->t > light_hit->distance) {
      const auto pdf = brdf ? PdfBrdf(mat, n, w_i, w_o) : PdfLight(x, w_i);
      const auto other_pdf =
          !brdf ? PdfBrdf(mat, n, w_i, w_o) : PdfLight(x, w_i);
      if (dot(w_i, n) <= 0 || pdf == 0) return vec3{0.f};
      const auto w = float(pow(pdf, 2) / (pow(pdf, 2) + pow(other_pdf, 2)));
      const auto f =
          light_hit->L_e * scene_.Brdf(mat, n, w_i, w_o) * dot(n, w_i);
      if constexpr (brdf) {
        return f * w / pdf;
      } else {
        return f * w / pdf / float(scene_.lights.size());
      }
    }
  }
  return vec3{0.f};
}

template <bool russian_roulette, Sampling sampling, bool mis>
glm::vec3 PathIntegrator<russian_roulette, sampling, mis>::LightDirect(
    const glm::vec3 &x, const glm::vec3 &n, const glm::vec3 &w_o,
    const MaterialRef &mat) const {
  using namespace glm;

  auto radiance = vec3{0.f};

  if constexpr (mis) {
    radiance += MISSample<true>(x, n, SampleBrdf(mat, n, w_o), w_o, mat);
    for (const auto &light : scene_.lights) {
      const auto sample = light->GetSample(x, SampleSquare());
      const auto w_i = normalize(sample.light - x);
      radiance += MISSample<false>(x, n, w_i, w_o, mat);
    }
  } else {
    for (const auto &light : scene_.lights) {
      const auto sample = light->GetSample(x, SampleSquare());
      const auto w_i = normalize(sample.light - x);
      const auto n_l = normalize(sample.normal);
      const auto w_i_n = dot(w_i, n), w_i_n_l = dot(w_i, n_l);
      if (w_i_n < 0 || w_i_n_l < 0) continue;
      if (const auto shadow_hit = scene_.TraceShapes(sample.GetShadowRay(x));
          !shadow_hit || shadow_hit->t > sample.distance) {
        const auto G = w_i_n * w_i_n_l / length2(sample.light - x);
        radiance += sample.radiance * sample.jacobian *
                    scene_.Brdf(mat, n, w_i, w_o) * G;
      }
    }
  }

  return radiance;
}
