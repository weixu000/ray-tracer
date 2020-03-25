#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <RayTracer/DirectionalLight.h>
#include <RayTracer/Light.h>
#include <RayTracer/Material.h>
#include <RayTracer/Mesh.h>
#include <RayTracer/PointLight.h>
#include <RayTracer/Renderer.h>
#include <RayTracer/Scene.h>
#include <RayTracer/Sphere.h>

using namespace std;

static Scene scene;

void OutputPPM(const vector<glm::u8vec3> &image) {
  ofstream fs(scene.output_file, ios::out | ios::binary);
  fs << "P6" << endl
     << scene.width << " " << scene.height << endl
     << 255 << endl;
  fs.write(reinterpret_cast<const char *>(image.data()),
           scene.width * scene.height * sizeof(glm::u8vec3));
}

int main(int argc, char *argv[]) {
  scene.output_file = "test.ppm";

  scene.width = scene.height = 400;
  scene.camera = Camera(glm::vec3(-5.f), glm::vec3(0.f, 0.f, 0.f),
                        glm::vec3(0.f, 1.f, 0.f), 75);

  Material material;
  material.ambient = glm::vec3(1.f);
  auto s = Sphere(glm::translate(glm::vec3(0.f)), material,
                  glm::vec3(0.f, 0.f, 0.f), 1.f);
  scene.geometries.push_back(make_unique<Sphere>(move(s)));

  Renderer renderer;
  auto image = renderer.Render(scene);
  OutputPPM(image);

  return EXIT_SUCCESS;
}