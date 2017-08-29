#ifndef OBJECTS_H
#define OBJECTS_H

#include <vector>
#include "glm/common.hpp"
#include <string>
#include "glm/matrix.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "image.h"

namespace ImageFilters{
    unsigned char * horLPF(Image &im);
    unsigned char * verLPF(Image &im);
}

struct Vertex{
    glm::vec3 Pos;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct VertInd{
    Vertex v;
    unsigned int ind;
};

struct vsComp{
    inline bool operator() (const Vertex& struct1, const Vertex& struct2)
    {
        return(struct1.Pos==struct2.Pos);
    }
};

struct VertSort
{
    inline bool operator() (const VertInd& struct1, const VertInd& struct2)
    {
        if(struct1.v.Pos.x!=struct2.v.Pos.x)
            return (struct1.v.Pos.x < struct2.v.Pos.x);
        else if(struct1.v.Pos.y!=struct2.v.Pos.y)
            return (struct1.v.Pos.y<struct2.v.Pos.y);
        else
            return(struct1.v.Pos.z<struct2.v.Pos.z);
    }
};

struct TextureS{
    unsigned int id;
    std::string type;
    aiString path;
};
struct Pair{
    std::pair<unsigned int, unsigned int> vecPair;
    Vertex contVert;
    glm::mat4 Q;
    long double cost;
};

struct pairSort
{
    inline bool operator() (const std::pair<unsigned int, unsigned int>& struct1, const std::pair<unsigned int, unsigned int>& struct2)
    {
        if(struct1.first!=struct2.first)
            return (struct1.first < struct2.first);
        else
            return(struct1.second<struct2.second);
    }
};


struct pairComp{
    inline bool operator() (const std::pair<unsigned int, unsigned int>& struct1, const std::pair<unsigned int, unsigned int>& struct2)
    {
        return(struct1.first==struct2.first && struct1.second==struct2.second);
    }
};

struct less_than_cost
{
    inline bool operator() (const Pair& struct1, const Pair& struct2)
    {
        return (struct1.cost < struct2.cost);
    }
};



#endif // OBJECTS_H
