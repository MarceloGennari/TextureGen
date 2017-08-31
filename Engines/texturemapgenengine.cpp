#include "textureengine.h"
#include "camera.h"
#include "../model.h"
#include "objects.h"

#include <sstream>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

void TextureEngine::TextureMapGenEngine::getTextureCoords(std::vector<Vertex> &vs, std::vector<unsigned int> &ind, const std::string &frN){

    // Getting the Key Frames
    std::vector<Frame *>  frames = TextureEngine::SaptiotemporalEngine::temporalSampling(30,10,504);

    std::string nr = SSTR(frames[15]->frameNr);

    Camera::getCam()->positionCameraFrN(nr);
    Image *im  = frames[15]->frame;

    // Projecting the Vertices to the RGB coordinate
    for(int k =0; k<vs.size(); k++){
        glm::vec3 posCamCoord = glm::vec3((Camera::getCam()->getView())*glm::vec4(vs[k].Pos, 1.0f));

        if(abs(posCamCoord.z) > 0.1f)
            continue;

        glm::vec2 pos = project(posCamCoord);
        pos = glm::vec2(floor(pos.x), floor(pos.y));

        //For some reason, the image ends up bein a bit displaced. This accounts for that
        pos.x = pos.x - 64;
        pos.y = pos.y + 3;


        if(pos.x<=640&&pos.x>0&&pos.y<=480&&pos.y>0){
            pos.x = 641-pos.x;
            im->setPixel(pos.x, pos.y, 255, 0, 0);
        }

        vs[k].TexCoords.x = pos.x;
        vs[k].TexCoords.y = pos.y;
    }


    //im->Display();

}

glm::vec2 TextureEngine::TextureMapGenEngine::project(glm::vec3 &vs){
    return glm::vec2(((vs.x*Camera::getCam()->getFocal().x)/vs.z)+Camera::getCam()->getPrinc().x,
                     ((vs.y*Camera::getCam()->getFocal().y)/vs.z)+Camera::getCam()->getPrinc().y);
}
