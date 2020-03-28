#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <raytracer/ImageWriter.hpp>
#include <raytracer/Material.hpp>
#include <raytracer/Sampler.hpp>
#include <raytracer/Scene.hpp>
#include <raytracer/lights/DirectionalLight.hpp>
#include <raytracer/lights/Light.hpp>
#include <raytracer/lights/PointLight.hpp>
#include <raytracer/shapes/Mesh.hpp>
#include <raytracer/shapes/Sphere.hpp>

using namespace std;

static Scene scene;
static vector<glm::mat4> transform_stack;
static std::vector<glm::ivec3> triangle_list;
static Material current_material;

static glm::mat4 StackMatrices() {
  glm::mat4 m(1.0f);
  for (const auto &x : transform_stack) {
    m *= x;
  }
  return m;
}

static void FinishMesh() {
  if (!triangle_list.empty()) {
    Mesh m(StackMatrices(), scene.verts, triangle_list);
    scene.primitives.emplace_back(make_unique<Mesh>(move(m)), current_material);
    triangle_list.clear();
  }
}

template <class CharT, class Traits, glm::length_t L, typename T,
          glm::qualifier Q>
static std::basic_istream<CharT, Traits> &
operator>>(std::basic_istream<CharT, Traits> &is, glm::vec<L, T, Q> &v) {
  for (int i = 0; i < v.length(); ++i) {
    is >> v[i];
  }
  return is;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Incorrect command-line options" << endl;
    return EXIT_FAILURE;
  }
  string input_path = argv[1];
  ifstream fs(input_path);
  if (!fs.is_open()) {
    cerr << "Cannot open input file: " << input_path << endl;
    return EXIT_FAILURE;
  }
  cout << "Parsing: " << input_path << endl;
  string line;
  while (getline(fs, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }
    istringstream ss(line);
    string command;
    ss >> command;
    if (command == "size") {
      ss >> scene.width >> scene.height;
    } else if (command == "maxdepth") {
      ss >> scene.max_depth;
    } else if (command == "output") {
      ss >> scene.output_file;
    } else if (command == "camera") {
      glm::vec3 look_from, look_at, up;
      float fov;
      ss >> look_from >> look_at >> up >> fov;
      scene.camera = Camera(look_from, look_at, up, fov);
    } else if (command == "sphere") {
      glm::vec3 position;
      float rad;
      ss >> position >> rad;
      auto s = Sphere(StackMatrices(), position, rad);
      scene.primitives.emplace_back(make_unique<Sphere>(move(s)),
                                    current_material);
    } else if (command == "maxverts") {
      ss >> scene.max_num_verts;
    } else if (command == "vertex") {
      glm::vec3 v;
      ss >> v;
      scene.verts.push_back(v);
    } else if (command == "tri") {
      glm::ivec3 t;
      ss >> t;
      triangle_list.push_back(t);
    } else if (command == "maxvertnorms") {
      continue; // TODO: implement triangle-with-normal
    } else if (command == "vertexnormal") {
      continue; // TODO: implement triangle-with-normal
    } else if (command == "trinormal") {
      continue; // TODO: implement triangle-with-normal
    } else if (command == "translate") {
      FinishMesh();
      glm::vec3 v;
      ss >> v;
      transform_stack.back() = glm::translate(transform_stack.back(), v);
    } else if (command == "rotate") {
      FinishMesh();
      glm::vec3 v;
      float angle;
      ss >> v >> angle;
      transform_stack.back() =
          glm::rotate(transform_stack.back(), glm::radians(angle), v);
    } else if (command == "scale") {
      FinishMesh();
      glm::vec3 v;
      ss >> v;
      transform_stack.back() = glm::scale(transform_stack.back(), v);
    } else if (command == "pushTransform") {
      FinishMesh();
      transform_stack.emplace_back(1.0f);
    } else if (command == "popTransform") {
      FinishMesh();
      transform_stack.pop_back();
    } else if (command == "directional") {
      glm::vec3 direction, color;
      ss >> direction >> color;
      scene.lights.push_back(make_unique<DirectionalLight>(color, direction));
    } else if (command == "point") {
      glm::vec3 position, color;
      ss >> position >> color;
      scene.lights.push_back(make_unique<PointLight>(color, position));
    } else if (command == "attenuation") {
      ss >> Light::attenuation;
    } else if (command == "ambient") {
      FinishMesh();
      ss >> current_material.ambient;
    } else if (command == "diffuse") {
      FinishMesh();
      ss >> current_material.diffuse;
    } else if (command == "specular") {
      FinishMesh();
      ss >> current_material.specular;
    } else if (command == "shininess") {
      FinishMesh();
      ss >> current_material.shininess;
    } else if (command == "emission") {
      FinishMesh();
      ss >> current_material.emission;
    } else {
      cerr << "Unknown command in input: " << line << endl;
      return EXIT_FAILURE;
    }
  }
  FinishMesh();

  cout << "Rendering..." << endl;
  Sampler renderer;
  auto image = renderer.Render(scene);
  ImageWriter::WriteTo(scene.output_file, scene.width, scene.height, image);

  return EXIT_SUCCESS;
}