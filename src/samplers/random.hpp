#pragma once

#include <random>

inline float Random() {
  static thread_local std::default_random_engine gen{std::random_device()()};
  static thread_local std::uniform_real_distribution<float> dist;
  return dist(gen);
}
