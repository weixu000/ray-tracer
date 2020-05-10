#pragma once

#include <cstddef>

enum class BRDF { Phong, GGX };

struct MaterialRef {
  BRDF type;
  size_t id;
};
