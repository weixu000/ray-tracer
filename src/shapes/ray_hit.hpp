#pragma once

#include "../materials/material_ref.hpp"
#include "local_info.hpp"

/**
 * local info of the primitive hit by the ray
 */
struct RayHit : public LocalInfo {
  MaterialRef mat;
};