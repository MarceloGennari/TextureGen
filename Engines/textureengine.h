#ifndef TEXTUREENGINE_H
#define TEXTUREENGINE_H

#include "mesh.h"
#include "objects.h"
#include "image.h"
#include <iostream>

namespace TextureEngine{

    namespace SurfaceSimplificationEngine{
        void Optimize(std::vector<Vertex> &vs, std::vector<unsigned int> &ind);
        void Optimize2(std::vector<Vertex> &vs, std::vector<unsigned int> &ind);
        void unRepVert(std::vector<Vertex> &vs, std::vector<unsigned int> &ind);
        void changeVert(std::vector<Pair> &Pairs, std::vector<Vertex> &vs, std::vector<unsigned int> &ind, std::vector<glm::mat4> &listQ);
        void changeVert2(std::vector<Pair> &Pairs, std::vector<Vertex> &vs, std::vector<unsigned int> &ind, std::vector<glm::mat4> &listQ);


        std::vector<Pair *> formPairList(std::vector<Vertex *> &vs, std::vector<std::pair<unsigned int, unsigned int> > &pairs, std::vector<glm::mat4 *> &Q);
        std::vector<glm::mat4 *> calcQMatrices(std::vector<Vertex *> &vs, std::vector<unsigned int> &ind);
        void changeVert(std::vector<Pair *> &Pairs, std::vector<Vertex *> &vs, std::vector<unsigned int> &ind, std::vector<glm::mat4 *> &listQ);

        std::vector<std::pair<unsigned int, unsigned int> > PairSelection(std::vector<unsigned int> &ind);
        std::vector<std::pair<unsigned int, unsigned int> > PairSelection2(std::vector<unsigned int> &ind);
        std::vector<Pair> formPairList(std::vector<Vertex> &vs, std::vector<std::pair<unsigned int, unsigned int> > &pairs, std::vector<glm::mat4> &Q);
        std::vector<glm::mat4> calcQMatrices(std::vector<Vertex> &vs, std::vector<unsigned int> &ind);
        std::vector<glm::mat4> calcQMatrices2(std::vector<Vertex> &vs, std::vector<unsigned int> &ind);
    }

    namespace SaptiotemporalEngine{
        std::vector<Frame *> temporalSampling(int sigMax, int sigMin, int nrFrames);
    }

    namespace TextureMapGenEngine {
        void getTextureCoords(std::vector<Vertex> &vs, std::vector<unsigned int> &ind, Frame *frame, int frNr, int totalFr);
        glm::vec2 project(glm::vec3 &vs);
        std::vector<std::vector<float> > zBuffering(std::vector<Vertex> &vs, std::vector<unsigned int> &ind, std::vector<std::vector<std::vector<Vertex *> > > &verticesInPixel, Frame * frame, std::vector<int> &faces);
    }


}


#endif // TEXTUREENGINE_H
