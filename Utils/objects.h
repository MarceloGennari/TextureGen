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



namespace ImageProcessing{

    namespace ImageFilters{
        unsigned char * horLPF(Image &im);
        unsigned char * verLPF(Image &im);
    }

    unsigned char * absVerDiff(Image &im);
    unsigned char * absHorDiff(Image &im);
    float calcBlur(Image &im);
}
struct Face{
    int FaceNr; // This is going to hold the face Number (which will be correspondent to the position of the face in the ind variable
    int FrAssigned; // This is going to hold the number of the frame that this face is assigned to in case all of its vertices has a texture coordinate
};

struct Vertex{
    glm::vec3 Pos; // Position of the vector in 3D
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec2 TexCoords2; // Additional coordinates for the case of zBuffering
    std::vector<int> faces; // This is a list of all of the faces that this vertex is part of
    float zDepth; // This is the zDepth of when this vertex is projected used in TextureGenEngine
    bool assigned;
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

struct Frame{
    // Note that frame #1 has the pose #2
    // So that frame #503 has the pose #504
    Image *frame;
    float blur;
    int frameNr;
};

struct less_than_blur
{
    inline bool operator() (const Frame* struct1, const Frame* struct2)
    {
        return (struct1->blur < struct2->blur);
    }
};

struct less_than_Nr
{
    inline bool operator() (const Frame *struct1, const Frame* struct2)
    {
        return (struct1->frameNr < struct2->frameNr);
    }
};


#endif // OBJECTS_H
