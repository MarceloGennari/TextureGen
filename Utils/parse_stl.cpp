#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>

#include "parse_stl.h"

namespace stl {

  float parse_float(std::ifstream& s) {
    char f_buf[sizeof(float)];
    s.read(f_buf, 4);
    float* fptr = (float*) f_buf;
    return *fptr;
  }

  Vertex parse_point(std::ifstream& s) {
    float x = parse_float(s);
    float y = parse_float(s);
    float z = parse_float(s);
    glm::vec3 Pos = glm::vec3(x,y,z);
    return Vertex(Pos);
  }

void parse_stl(const std::string& stl_path, std::vector<Vertex> &vs, std::vector<unsigned int> &ind) {
    std::ifstream stl_file(stl_path.c_str(), std::ios::in | std::ios::binary);
    if (!stl_file) {
      std::cout << "ERROR: COULD NOT READ FILE" << std::endl;
      assert(false);
    }

    char header_info[80] = "";
    char n_triangles[4];
    stl_file.read(header_info, 80);
    stl_file.read(n_triangles, 4);

    unsigned int* r = (unsigned int*) n_triangles;
    unsigned int num_triangles = *r;
    for (unsigned int i = 0; i < num_triangles; i++) {
      auto normal = parse_point(stl_file);
      auto v1 = parse_point(stl_file);
      auto v2 = parse_point(stl_file);
      auto v3 = parse_point(stl_file);
      vs.push_back(v1);
      vs.push_back(v2);
      vs.push_back(v3);
      ind.push_back(3*i);
      ind.push_back(3*i+1);
      ind.push_back(3*i+2);
      char dummy[2];
      stl_file.read(dummy, 2);
    }
  }

}
