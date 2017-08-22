#ifndef OBJECTS_H
#define OBJECTS_H

struct Vertex{
    glm::vec3 Pos;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
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


struct less_than_cost
{
    inline bool operator() (const Pair& struct1, const Pair& struct2)
    {
        return (struct1.cost < struct2.cost);
    }
};



#endif // OBJECTS_H
