#pragma once

#include <array>
#include <tuple>

#include "../samplers/sampling.hpp"
#include "material.hpp"

template <typename... Ts>
class ComposedMaterial : public Material {
 public:
  ComposedMaterial(Ts &&... args) : materials_(std::forward<Ts>(args)...) {}

  friend bool operator==(const ComposedMaterial &x, const ComposedMaterial &y) {
    return x.materials_ == y.materials_;
  }

  friend bool operator!=(const ComposedMaterial &x, const ComposedMaterial &y) {
    return !(x == y);
  }

  glm::vec3 Brdf(const glm::vec3 &n, const glm::vec3 &w_i,
                 const glm::vec3 &w_o) const override {
    return std::apply(
        [&](const auto &... m) { return (m.Brdf(n, w_i, w_o) + ...); },
        materials_);
  }

  glm::vec3 Sample(const glm::vec3 &n, const glm::vec3 &w_i) const override {
    using namespace glm;
    const auto power = Power(n, w_i);
    const auto probs = std::apply(
        [&](const auto &... m) {
          return std::array<float, sizeof...(Ts)>{m.Power(n, w_i) / power...};
        },
        materials_);
    auto t = Random();
    size_t i = 0;
    for (; i < sizeof...(Ts); ++i) {
      t -= probs[i];
      if (t <= 0)
        return Call(i, [&](const auto &m) { return m.Sample(n, w_i); });
    }
    return Call(sizeof...(Ts) - 1,
                [&](const auto &m) { return m.Sample(n, w_i); });
  }

  float Pdf(const glm::vec3 &n, const glm::vec3 &w_i,
            const glm::vec3 &w_o) const override {
    const auto power = Power(n, w_i);
    return std::apply(
        [&](const auto &... m) {
          return ((m.Pdf(n, w_i, w_o) * m.Power(n, w_i) / power) + ...);
        },
        materials_);
  }

  float Power(const glm::vec3 &n, const glm::vec3 &w_i) const override {
    return std::apply(
        [&](const auto &... m) { return (m.Power(n, w_i) + ...); }, materials_);
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
