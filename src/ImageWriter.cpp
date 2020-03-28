#include "raytracer/ImageWriter.hpp"

#include <stb_image_write.h>

void ImageWriter::WriteTo(const std::string &file_path, int w, int h,
                          const std::vector<glm::u8vec3> &image) {
  auto ext = file_path.substr(file_path.size() - 4);
  if (ext == ".png") {
    stbi_write_png(file_path.c_str(), w, h, 3, image.data(),
                   w * sizeof(glm::u8vec3));
  } else {
    // TODO: More format
  }
}
