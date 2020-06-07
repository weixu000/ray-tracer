#include <tiny_obj_loader.h>

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "integrators/path_tracer.hpp"
#include "lights/quad_light.hpp"

using namespace std;
using namespace glm;

namespace {
mat4 StackMatrices(const vector<mat4> &transform_stack) {
  mat4 m(1.0f);
  for (const auto &x : transform_stack) {
    m *= x;
  }
  return m;
}

template <class CharT, class Traits, length_t L, typename T, qualifier Q>
basic_istream<CharT, Traits> &operator>>(basic_istream<CharT, Traits> &is,
                                         vec<L, T, Q> &v) {
  for (int i = 0; i < v.length(); ++i) {
    is >> v[i];
  }
  return is;
}

using Options = unordered_map<string, string>;

auto GetMaterial(Scene &scene, const string &material_type, const vec3 &k_d,
                 const vec3 &k_s, float s, float alpha, float n, float albedo,
                 float softness) {
  if (material_type == "phong")
    return scene.AddMaterial<Phong>(k_d, k_s, s);
  else if (material_type == "ggx")
    return scene.AddMaterial<GGXReflection>(k_d, k_s, alpha);
  else if (material_type == "refractive")
    return scene.AddMaterial<GGXRefraction>(k_s, alpha);
  else if (material_type == "subsurface")
    return scene.AddMaterial<SubSurfaceMaterial>(albedo, n, softness);
  else {
    cerr << "Unkown material:" << material_type << endl;
    exit(EXIT_FAILURE);
  }
}

auto GetTriangles(const uvec3 &t, const vector<vec3> &verts,
                  const mat4 &transform) {
  return array<vec3, 3>{
      transform * vec4(verts[t[0]], 1.f),
      transform * vec4(verts[t[1]], 1.f),
      transform * vec4(verts[t[2]], 1.f),
  };
}

auto GetSphere(const vec3 &p, float r, const mat4 &transform) {
  const auto world = scale(translate(transform, p), vec3(r));
  return world;
}

auto GetObj(const string &inputfile, const mat4 &transform,
            const MaterialRef &mat) {
  tinyobj::attrib_t attrib;
  vector<tinyobj::shape_t> shapes;
  vector<tinyobj::material_t> materials;

  string warn;
  string err;

  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                              inputfile.c_str());

  if (!warn.empty()) {
    cout << warn << endl;
  }

  if (!err.empty()) {
    cerr << err << endl;
  }

  if (!ret) {
    exit(1);
  }

  Mesh mesh;
  mesh.material = mat;
  for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
    mesh.vertices.emplace_back(transform * vec4{attrib.vertices[i + 0],
                                                attrib.vertices[i + 1],
                                                attrib.vertices[i + 2], 1.f});
  }

  // Loop over shapes
  for (auto &shape : shapes) {
    // Loop over faces(polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
      int fv = shape.mesh.num_face_vertices[f];
      if (fv != 3) {
        cerr << "Not a triangular mesh" << endl;
        exit(1);
      }
      mesh.indices.emplace_back(
          shape.mesh.indices[index_offset + 0].vertex_index,
          shape.mesh.indices[index_offset + 1].vertex_index,
          shape.mesh.indices[index_offset + 2].vertex_index);
      index_offset += fv;
    }
  }
  return mesh;
}

auto LoadScene(ifstream &fs) {
  Scene scene;
  Options others;
  vector<vec3> verts;
  vector<mat4> transform_stack;
  vec3 k_d, k_s;
  float s, alpha, n, albedo, softness;
  string material_type = "phong";

  string line;
  while (getline(fs, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }
    istringstream ss(line);
    string command;
    ss >> command;
    if (command == "sphere") {
      vec3 position;
      float rad;
      ss >> position >> rad;
      scene.geometries.emplace_back(
          Sphere{GetSphere(position, rad, StackMatrices(transform_stack)),
                 GetMaterial(scene, material_type, k_d, k_s, s, alpha, n,
                             albedo, softness)});
    } else if (command == "maxverts") {
      continue;  // Not used
    } else if (command == "vertex") {
      vec3 v;
      ss >> v;
      verts.push_back(v);
    } else if (command == "tri") {
      uvec3 t;
      ss >> t;
      const auto triangle =
          GetTriangles(t, verts, StackMatrices(transform_stack));
      scene.geometries.emplace_back(
          Mesh{{begin(triangle), end(triangle)},
               {{0, 1, 2}},
               GetMaterial(scene, material_type, k_d, k_s, s, alpha, n, albedo,
                           softness)});
    } else if (command == "obj") {
      string file_path;
      getline(ss >> ws, file_path);
      scene.geometries.emplace_back(
          GetObj(file_path, StackMatrices(transform_stack),
                 GetMaterial(scene, material_type, k_d, k_s, s, alpha, n,
                             albedo, softness)));
    } else if (command == "maxvertnorms") {
      continue;  // TODO: implement triangle-with-normal
    } else if (command == "vertexnormal") {
      continue;  // TODO: implement triangle-with-normal
    } else if (command == "trinormal") {
      continue;  // TODO: implement triangle-with-normal
    } else if (command == "translate") {
      vec3 v;
      ss >> v;
      transform_stack.back() = translate(transform_stack.back(), v);
    } else if (command == "rotate") {
      vec3 v;
      float angle;
      ss >> v >> angle;
      transform_stack.back() =
          rotate(transform_stack.back(), radians(angle), v);
    } else if (command == "scale") {
      vec3 v;
      ss >> v;
      transform_stack.back() = scale(transform_stack.back(), v);
    } else if (command == "pushTransform") {
      transform_stack.emplace_back(1.0f);
    } else if (command == "popTransform") {
      transform_stack.pop_back();
    } else if (command == "directional") {
      cerr << "Directional light removed form implementation" << endl;
      exit(EXIT_FAILURE);
    } else if (command == "point") {
      cerr << "Point light removed form implementation" << endl;
      exit(EXIT_FAILURE);
    } else if (command == "quadLight") {
      vec3 v0, e1, e2, intensity;
      ss >> v0 >> e1 >> e2 >> intensity;
      scene.lights.push_back(make_unique<QuadLight>(intensity, v0, e1, e2));
    } else if (command == "diffuse") {
      ss >> k_d;
    } else if (command == "specular") {
      ss >> k_s;
    } else if (command == "shininess") {
      ss >> s;
    } else if (command == "roughness") {
      ss >> alpha;
    } else if (command == "ior") {
      ss >> n;
    } else if (command == "albedo") {
      ss >> albedo;
    } else if (command == "softness") {
      ss >> softness;
    } else if (command == "brdf") {
      getline(ss >> ws, material_type);
    } else {
      string option;
      getline(ss >> ws, option);
      others[command] = option;
    }
  }
  return make_tuple(move(scene), move(others));
}

auto LoadCamera(const Options &options) {
  istringstream ss;

  int width, height;
  ss.str(options.at("size"));
  ss >> width >> height;
  vec3 look_from, look_at, up;
  float fov;
  ss.clear();
  ss.str(options.at("camera"));
  ss >> look_from >> look_at >> up >> fov;
  return Camera(look_from, look_at, up, fov, width, height);
}

template <typename C, typename K, typename V>
V GetDefault(const C &m, const K &key, const V &defval) {
  const auto it = m.find(key);
  if (it == m.end()) return defval;
  istringstream ss(it->second);
  V ret;
  ss >> ret;
  return ret;
}

unique_ptr<Integrator> LoadIntegrator(const Options &options, Scene scene,
                                      Camera camera) {
  if (options.at("integrator") == "pathtracer") {
    if (GetDefault(options, "nexteventestimation", "off"s) == "off") {
      cerr << "Simple path tracer removed" << endl;
      exit(EXIT_FAILURE);
    }

    const auto mis =
        GetDefault(options, "nexteventestimation", "off"s) == "mis";
    if (mis) cout << "Multiple Importance Sampling enabled" << endl;

    const auto russian_roulette =
        GetDefault(options, "russianroulette", "off"s) == "on";
    if (russian_roulette) cout << "Russian Roulette enabled" << endl;

    const auto spp = GetDefault(options, "spp", 1);
    cout << "Sample per pixel: " << spp << endl;

    const auto max_depth = GetDefault(options, "maxdepth", 5);
    if (!russian_roulette) cout << "Max depth: " << max_depth << endl;

    const auto gamma = GetDefault(options, "gamma", 1.f);
    cout << "Gamma: " << gamma << endl;

    if (GetDefault(options, "importancesampling", "hemisphere"s) != "brdf") {
      cerr << "Importance sampling other than BRDF removed" << endl;
      exit(EXIT_FAILURE);
    }

    return make_unique<PathTracer>(russian_roulette, mis, max_depth, spp,
                                   move(scene), move(camera), gamma);
  } else {
    cerr << "Unknown integrator: " << options.at("integrator") << endl;
    exit(EXIT_FAILURE);
  }
}
}  // namespace

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "Incorrect command-line options" << endl;
    exit(EXIT_FAILURE);
  }
  string input_path = argv[1];
  ifstream fs(input_path);
  if (!fs.is_open()) {
    cerr << "Cannot open input file: " << input_path << endl;
    exit(EXIT_FAILURE);
  }
  cout << "Parsing: " << input_path << endl;

  auto [scene, options] = LoadScene(fs);
  auto camera = LoadCamera(options);
  const auto integrator = LoadIntegrator(options, move(scene), move(camera));

  const auto output_file = GetDefault(options, "output", ""s);
  if (output_file.empty()) {
    cerr << "Output file unspecified" << endl;
    exit(EXIT_FAILURE);
  }

  integrator->Render().WriteTo(output_file);
  cout << "Output: " << output_file << endl;
}