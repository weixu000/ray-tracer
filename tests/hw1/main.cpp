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
#include <RayTracer/Scene.h>
#include <RayTracer/Sphere.h>

using namespace std;

static Scene info;
static vector<glm::mat4> transform_stack;
static Mesh current_mesh;
static Material current_material;

static glm::mat4 stack_matrices() {
  glm::mat4 m(1.0f);
  for (const auto &x : transform_stack) {
    m *= x;
  }
  return m;
}

static void finish_mesh() {
  if (!current_mesh.triangles.empty()) {
    current_mesh.transform = stack_matrices();
    current_mesh.material = current_material;
    info.geometries.push_back(make_unique<Mesh>(move(current_mesh)));
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
      ss >> info.width >> info.height;
    } else if (command == "maxdepth") {
      ss >> info.depth;
    } else if (command == "output") {
      ss >> info.output_file;
    } else if (command == "camera") {
      ss >> info.camera.look_from >> info.camera.look_at >> info.camera.up >>
          info.camera.fov;
    } else if (command == "sphere") {
      Sphere s;
      ss >> s.position >> s.radius;
      s.transform = stack_matrices();
      s.material = current_material;
      info.geometries.push_back(make_unique<Sphere>(move(s)));
    } else if (command == "maxverts") {
      ss >> info.max_num_verts;
    } else if (command == "vertex") {
      glm::vec3 v;
      ss >> v;
      info.verts.push_back(v);
    } else if (command == "tri") {
      glm::ivec3 t;
      ss >> t;
      current_mesh.triangles.push_back(t);
    } else if (command == "maxvertnorms") {
      continue; // TODO: implement triangle-with-normal
    } else if (command == "vertexnormal") {
      continue; // TODO: implement triangle-with-normal
    } else if (command == "trinormal") {
      continue; // TODO: implement triangle-with-normal
    } else if (command == "translate") {
      finish_mesh();
      glm::vec3 v;
      ss >> v;
      glm::translate(transform_stack.back(), v);
    } else if (command == "rotate") {
      finish_mesh();
      glm::vec3 v;
      float angle;
      ss >> v >> angle;
      glm::rotate(transform_stack.back(), angle, v);
    } else if (command == "scale") {
      finish_mesh();
      glm::vec3 v;
      ss >> v;
      glm::scale(transform_stack.back(), v);
    } else if (command == "pushTransform") {
      finish_mesh();
      transform_stack.emplace_back(1.0f);
    } else if (command == "popTransform") {
      finish_mesh();
      transform_stack.pop_back();
    } else if (command == "directional") {
      DirectionalLight light{};
      ss >> light.direction >> light.color;
      info.lights.push_back(make_unique<DirectionalLight>(light));
    } else if (command == "point") {
      PointLight light{};
      ss >> light.position >> light.color;
      info.lights.push_back(make_unique<PointLight>(light));
    } else if (command == "attenuation") {
      ss >> Light::attenuation;
    } else if (command == "ambient") {
      finish_mesh();
      ss >> current_material.ambient;
    } else if (command == "diffuse") {
      finish_mesh();
      ss >> current_material.diffuse;
    } else if (command == "specular") {
      finish_mesh();
      ss >> current_material.specular;
    } else if (command == "shininess") {
      finish_mesh();
      ss >> current_material.shininess;
    } else if (command == "emission") {
      finish_mesh();
      ss >> current_material.emission;
    } else {
      cerr << "Unknown command in input: " << line << endl;
      return EXIT_FAILURE;
    }
  }
  finish_mesh();
  return EXIT_SUCCESS;
}