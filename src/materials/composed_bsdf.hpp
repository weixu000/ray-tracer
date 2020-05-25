#pragma once

#include <tuple>

#include "../samplers/sampling.hpp"
#include "bsdf.hpp"

template <typename... Ts>
struct ComposedBSDF : public BSDF {
  glm::vec3 Value(const glm::vec3 &w_i, const glm::vec3 &w_o) const override {
    return std::apply(
        [&](const auto &... m) { return (m.Value(w_i, w_o) + ...); }, bsdfs);
  }

  glm::vec3 Sample(const glm::vec3 &w_i) const override {
    using namespace glm;
    const auto weight = Weight(w_i);
    const auto probs = std::apply(
        [&](const auto &... m) {
          return std::array<float, sizeof...(Ts)>{m.Weight(w_i) / weight...};
        },
        bsdfs);
    auto t = Random();
    size_t i = 0;
    for (; i < sizeof...(Ts); ++i) {
      t -= probs[i];
      if (t <= 0) return Call(i, [&](const auto &m) { return m.Sample(w_i); });
    }
    return Call(sizeof...(Ts) - 1,
                [&](const auto &m) { return m.Sample(w_i); });
  }

  float Pdf(const glm::vec3 &w_i, const glm::vec3 &w_o) const override {
    const auto weight = Weight(w_i);
    return std::apply(
        [&](const auto &... m) {
          return ((m.Pdf(w_i, w_o) * m.Weight(w_i) / weight) + ...);
        },
        bsdfs);
  }

  float Weight(const glm::vec3 &w_i) const override {
    return std::apply([&](const auto &... m) { return (m.Weight(w_i) + ...); },
                      bsdfs);
  }

  std::tuple<Ts...> bsdfs;

 private:
  template <size_t I = 0, typename F>
  auto Call(size_t i, F f) const {
    if (i == I) {
      return f(std::get<I>(bsdfs));
    } else if constexpr (I + 1 < sizeof...(Ts)) {
      return Call<I + 1>(i, f);
    } else {
      throw std::runtime_error("Unknown type");
    }
  }
};
