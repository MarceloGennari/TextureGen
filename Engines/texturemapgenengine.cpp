#include "textureengine.h"
#include "camera.h"
#include "../model.h"
#include "objects.h"

#include <sstream>
#include <cmath>
#include <algorithm>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

void TextureEngine::TextureMapGenEngine::getTextureCoords(std::vector<Vertex> &vs, std::vector<unsigned int> &ind, Frame *frame){

    std::vector<unsigned int> verticesInsideImage;
    std::vector<float> depthPixels;
    depthPixels.resize(frame->frame->getHeight()*frame->frame->getWidth());

    // This creates a vector of vertices that project to the pixel x,y
    std::string nr = SSTR(frame->frameNr-1);
    Camera::getCam()->positionCameraFrN(nr);

    // Projecting the Vertices to the RGB coordinate
    for(int k =0; k<vs.size(); k++){
        glm::vec3 posCamCoord = glm::vec3(Camera::getCam()->getView()*glm::vec4(vs[k].Pos, 1.0f));
//        glm::vec4 posCamCoord2 = Camera::getCam()->getKMat()*posDepthCam;
//        glm::vec3 posCamCoord = glm::vec3(posCamCoord2.x/posCamCoord2.w, posCamCoord2.y/posCamCoord2.w, posCamCoord2.z/posCamCoord2.w);
        glm::vec2 pos = project(posCamCoord);
        pos = glm::vec2(round(pos.x), round(pos.y));

        pos.y = pos.y +5;
        pos.x = pos.x -70;

        if(pos.x<=640&&pos.x>0&&pos.y<=480&&pos.y>0 && std::abs(posCamCoord.z)<1.0f){

            pos.x = 641-pos.x;
            verticesInsideImage.push_back(k);
            vs[k].zDepth = posCamCoord.z;
            vs[k].TexCoords.x = pos.x/640;
            vs[k].TexCoords.y = pos.y/480;
        }
    }
}

glm::vec2 TextureEngine::TextureMapGenEngine::project(glm::vec3 &vs){
    return glm::vec2(((vs.x*Camera::getCam()->getFocal().x)/vs.z)+Camera::getCam()->getPrinc().x,
                     ((vs.y*Camera::getCam()->getFocal().y)/vs.z)+Camera::getCam()->getPrinc().y);
}

std::vector<Vertex *> TextureEngine::TextureMapGenEngine::zBuffering(std::vector<Vertex> &vs, std::vector<unsigned int> &ind){

    // This will hopefully solve the zBuffering problem from the texture generation
    /* The idea of the algorithm is:
     * Given all of the vertices and the faces projected in the image
     * We go iteratively and destroy the vertices that are behind the faces
     * For that we need to know:
     * Is the vertice inside one of the faces?
     * If this vertice is inside one of the faces, is it in front or behind the face?
     * If it is behind, then we set the texture coordinate to zero
     * If it is in front, the we set the texture coordinate to that value
     * */
}

bool isInsideTriangle(glm::vec2 TexCoord, std::vector<glm::vec2> Face){
    /* This just tests whether the point TexCoord is inside the Face (which is a vector of three points in the image)
     * This uses the method of Same Side Technique
     * */
}
