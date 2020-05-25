#pragma once

#include <array>
#include <tuple>

#include "../samplers/sampling.hpp"
#include "material.hpp"

template <size_t N>
struct ComposedBSDF : public BSDF {
  glm::vec3 Brdf(const glm::vec3 &w_i, const glm::vec3 &w_o) const override {
    auto x = glm::vec3{0.f};
    for (const auto m : bsdfs) {
      x += m->Brdf(w_i, w_o);
    }
    return x;
  }

  glm::vec3 Sample(const glm::vec3 &w_i) const override {
    using namespace glm;
    const auto power = Power(w_i);
    std::array<float, N> probs;
    for (size_t i = 0; i < N; ++i) {
      probs[i] = bsdfs[i]->Power(w_i) / power;
    }
    auto t = Random();
    size_t i = 0;
    for (; i < N; ++i) {
      t -= probs[i];
      if (t <= 0) return bsdfs[i]->Sample(w_i);
    }
    return bsdfs[N - 1]->Sample(w_i);
  }

  float Pdf(const glm::vec3 &w_i, const glm::vec3 &w_o) const override {
    const auto p = Power(w_i);
    float x = 0.f;
    for (const auto m : bsdfs) {
      x += m->Pdf(w_i, w_o) * m->Power(w_i) / p;
    }
    return x;
  }

  float Power(const glm::vec3 &w_i) const override {
    float x = 0.f;
    for (const auto m : bsdfs) {
      x += m->Power(w_i);
    }
    return x;
  }

  std::array<const BSDF *, N> bsdfs;
};

template <typename... Ts>
class ComposedMaterial : public Material {
 public:
  ComposedMaterial(Ts &&... args) : materials_(std::forward<Ts>(args)...) {}

  const BSDF *GetBSDF(const glm::vec3 &n) const override {
    thread_local ComposedBSDF<sizeof...(Ts)> bsdf;
    for (int i = 0; i < sizeof...(Ts); ++i) {
      bsdf.bsdfs[i] = Call(i, [&n](const auto &m) { return m.GetBSDF(n); });
    }
    return &bsdf;
  }

 private:
  template <size_t I = 0, typename F>
  auto Call(size_t i, F f) const {
    if (i == I) {
      return f(std::get<I>(materials_));
    } else if constexpr (I + 1 < sizeof...(Ts)) {
      return Call<I + 1>(i, f);
    } else {
      throw std::runtime_error("Unknown type");
    }
  }

  std::tuple<Ts...> materials_;
};
