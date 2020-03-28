#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

class ImageWriter {
public:
  static void WriteTo(const std::string &file_path, int w, int h,
                      const std::vector<glm::u8vec3> &image);
};
