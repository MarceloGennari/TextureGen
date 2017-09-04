#include "textureengine.h"
#include "camera.h"
#include "../model.h"
#include "objects.h"

#include <sstream>
#include <cmath>
#include <algorithm>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

bool sorting(Vertex *a, Vertex *b){ return a->zDepth<b->zDepth;}

void TextureEngine::TextureMapGenEngine::getTextureCoords(std::vector<Vertex> &vs, std::vector<unsigned int> &ind, Frame *frame){

    /* Proposed Algorithm:
     * Find the zDepth of each pixel projected and store the vertex in a vector
     * Find the vertices with the lowest z Depths (the closest to the camer)
     * Destroy the vertices with the further z Depths
     * */

    // This creates a vector of vertices that project to the pixel x,y
    std::vector<std::vector<Vertex *> > zBuffer;
    std::vector<std::vector<Vertex *> >::iterator it;
    zBuffer.resize(frame->frame->getHeight()*frame->frame->getWidth());

    std::string nr = SSTR(frame->frameNr);

    Camera::getCam()->positionCameraFrN(nr);

    // Projecting the Vertices to the RGB coordinate
    for(int k =0; k<vs.size(); k++){
        glm::vec3 posCamCoord = glm::vec3((Camera::getCam()->getView())*glm::vec4(vs[k].Pos, 1.0f));

        if(abs(posCamCoord.z) > 0.1f)
            continue;

        glm::vec2 pos = project(posCamCoord);
        pos = glm::vec2(floor(pos.x), floor(pos.y));

        //For some reason, the image ends up being a bit displaced. This accounts for that
        pos.x = pos.x - 64;
        pos.y = pos.y + 3;


        if(pos.x<=640&&pos.x>0&&pos.y<=480&&pos.y>0){
            // This means that the vertex is projected in the image
            // Therefore, we just add it ot the zBuffer
            int index = pos.x*pos.y;
            pos.x = 641-pos.x;
            zBuffer[index].push_back(&vs[k]);
            zBuffer[index][zBuffer[index].size()-1]->zDepth = posCamCoord.z;
            zBuffer[index][zBuffer[index].size()-1]->TexCoords.x = pos.x/640;
            zBuffer[index][zBuffer[index].size()-1]->TexCoords.y = pos.y/480;
        }
    }

    // Now that we have a list of all of the vertices assigned to each pixel, we eliminate those with the lowst zDepth value
    for(it =zBuffer.begin(); it <zBuffer.end(); it++){
            std::sort(it[0].begin(), it[0].end(), sorting);
    }
}

glm::vec2 TextureEngine::TextureMapGenEngine::project(glm::vec3 &vs){
    return glm::vec2(((vs.x*Camera::getCam()->getFocal().x)/vs.z)+Camera::getCam()->getPrinc().x,
                     ((vs.y*Camera::getCam()->getFocal().y)/vs.z)+Camera::getCam()->getPrinc().y);
}
