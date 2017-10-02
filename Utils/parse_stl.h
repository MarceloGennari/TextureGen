#ifndef PARSE_STL_H
#define PARSE_STL_H

#include <string>
#include <vector>
#include <objects.h>
namespace stl {

  struct triangle {
    Vertex normal;
    Vertex v1;
    Vertex v2;
    Vertex v3;
    triangle(Vertex normalp, Vertex v1p, Vertex v2p, Vertex v3p) :
      normal(normalp), v1(v1p), v2(v2p), v3(v3p) {}
  };

  std::ostream& operator<<(std::ostream& out, const triangle& t);

  struct stl_data {
    std::string name;
    std::vector<triangle> triangles;

    stl_data(std::string namep) : name(namep) {}
  };

  void parse_stl(const std::string& stl_path, std::vector<Vertex> &vs, std::vector<unsigned int> &ind);

}

#endif
