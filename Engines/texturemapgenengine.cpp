#include "textureengine.h"
#include "camera.h"
#include "../model.h"
#include "objects.h"

#include <sstream>
#include <cmath>
#include <algorithm>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

void TextureEngine::TextureMapGenEngine::getTextureCoords(std::vector<Vertex> &vs, std::vector<unsigned int> &ind, Frame *frame, int nF){

    // This creates a vector of vertices that project to the pixel x,y
    std::string nr = SSTR(frame->frameNr+1);
    Image *im = frame->frame;
    Camera::getCam()->positionCameraFrN(nr);

    // Projecting the Vertices to the RGB coordinate
    for(int k =0; k<vs.size(); k++){
        glm::vec3 posCamCoord = glm::vec3((Camera::getCam()->getView())*glm::vec4(vs[k].Pos, 1.0f));

        glm::vec2 pos = project(posCamCoord);
        pos = glm::vec2(floor(pos.x), floor(pos.y));


        if(pos.x<=640&&pos.x>0&&pos.y<=480&&pos.y>0){

            pos.x = 641-pos.x;
            vs[k].zDepth = posCamCoord.z;
            vs[k].TexCoords.x = pos.x/640;
            vs[k].TexCoords.y = pos.y/480;
            im->setPixel(pos.x, pos.y, 255, 0, 0);
        }
    }

    im->Display();
}

glm::vec2 TextureEngine::TextureMapGenEngine::project(glm::vec3 &vs){
    return glm::vec2(((vs.x*Camera::getCam()->getFocal().x)/vs.z)+Camera::getCam()->getPrinc().x,
                     ((vs.y*Camera::getCam()->getFocal().y)/vs.z)+Camera::getCam()->getPrinc().y);
}

std::vector<Vertex *> TextureEngine::TextureMapGenEngine::zBuffering(std::vector<Vertex> &vs, std::vector<unsigned int> &ind){

}
