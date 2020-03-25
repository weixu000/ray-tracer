#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    Mesh m(StackMatrices(), current_material, scene.verts, triangle_list);
    scene.geometries.push_back(make_unique<Mesh>(move(m)));
    triangle_list.clear();
  }
}

void OutputPPM(const vector<glm::u8vec3> &image) {
  ofstream fs(scene.output_file, ios::out | ios::binary);
  fs << "P6" << endl
     << scene.width << " " << scene.height << endl
     << 255 << endl;
  fs.write(reinterpret_cast<const char *>(image.data()),
           scene.width * scene.height * sizeof(glm::u8vec3));
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
      scene.output_file = "test.ppm";
      //      ss >> scene.output_file;
    } else if (command == "camera") {
      glm::vec3 look_from, look_at, up;
      float fov;
      ss >> look_from >> look_at >> up >> fov;
      scene.camera = Camera(look_from, look_at, up, fov);
    } else if (command == "sphere") {
      glm::vec3 position;
      float rad;
      ss >> position >> rad;
      auto s = Sphere(StackMatrices(), current_material, position, rad);
      scene.geometries.push_back(make_unique<Sphere>(move(s)));
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
      DirectionalLight light{};
      ss >> light.direction >> light.color;
      scene.lights.push_back(make_unique<DirectionalLight>(light));
    } else if (command == "point") {
      PointLight light{};
      ss >> light.position >> light.color;
      scene.lights.push_back(make_unique<PointLight>(light));
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
  Renderer renderer;
  auto image = renderer.Render(scene);
  OutputPPM(image);

  return EXIT_SUCCESS;
}