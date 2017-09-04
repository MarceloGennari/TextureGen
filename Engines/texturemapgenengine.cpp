#include "textureengine.h"
#include "camera.h"
#include "../model.h"
#include "objects.h"

#include <sstream>
#include <cmath>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

void TextureEngine::TextureMapGenEngine::getTextureCoords(std::vector<Vertex> &vs, std::vector<unsigned int> &ind, Frame *frame){

    // This is representing the z-buffer and it will prevent vertices behind faces to get a texture coordinate
    float *zBuffer = new float[frame->frame->getWidth()*frame->frame->getHeight()](); // The parenthesis initializes this buffer to 0
    std::vector<Vertex*> *zB = new std::vector<Vertex*>[frame->frame->getHeight()*frame->frame->getWidth()];

    std::string nr = SSTR(frame->frameNr);
    Camera::getCam()->positionCameraFrN(nr);

    // Projecting the Vertices to the RGB coordinate
    for(int k =0; k<vs.size(); k++){
        glm::vec3 posCamCoord = glm::vec3((Camera::getCam()->getView())*glm::vec4(vs[k].Pos, 1.0f));

        glm::tvec2<int> pos = project(posCamCoord);
        pos = glm::tvec2<int>(floor(pos.x), floor(pos.y));

        //For some reason, the image ends up being a bit displaced. This accounts for that
        pos.x = pos.x - 64;
        pos.y = pos.y + 3;


        if(pos.x<=640&&pos.x>0&&pos.y<=480&&pos.y>0){
            pos.x = 641-pos.x;

            // z-Buffering to make sure the "closest" pixel gets the textCoord;
            /* Maybe two vertices that are very close have the same Texture Coordinate
             * So the value in the right hand side of the inequation below accounts for that
             * The idea is just to prevent vertices that are "behind" to get the same coords
             * */

            if(zBuffer[pos.x*pos.y]==0 || glm::distance(vs[k].Pos,zB[pos.x*pos.y][0]->Pos) < 0.3f){
                // If no other vertex has that pixel assigned
                // Assign it to that pixel
                zBuffer[pos.x*pos.y] = posCamCoord.z;
                zB[pos.x*pos.y].push_back(&vs[k]);
                zB[pos.x*pos.y][zB[pos.x*pos.y].size()-1]->TexCoords.x = ((float)pos.x)/640;
                zB[pos.x*pos.y][zB[pos.x*pos.y].size()-1]->TexCoords.y = ((float)pos.y)/480;

            } else if(zBuffer[pos.x*pos.y] < posCamCoord.z){
                // If that Pixel already has a vertex assigned to it and the new vertex is "closer"
                // Update Buffers:
                zBuffer[pos.x*pos.y] = posCamCoord.z;
                for(int w = 0; w<zB[pos.x*pos.y].size(); w++){
                    zB[pos.x*pos.y][w]->TexCoords.x = 0;
                    zB[pos.x*pos.y][w]->TexCoords.y = 0;
                }
                zB[pos.x*pos.y].clear();
                zB[pos.x*pos.y].push_back(&vs[k]);

                // Assign that vertex the value of the pixels
                zB[pos.x*pos.y][zB[pos.x*pos.y].size()-1]->TexCoords.x = ((float)pos.x)/640;
                zB[pos.x*pos.y][zB[pos.x*pos.y].size()-1]->TexCoords.y = ((float)pos.y)/480;
            }

        }
    }


    //im->Display();
    delete(zBuffer);
}

glm::vec2 TextureEngine::TextureMapGenEngine::project(glm::vec3 &vs){
    return glm::vec2(((vs.x*Camera::getCam()->getFocal().x)/vs.z)+Camera::getCam()->getPrinc().x,
                     ((vs.y*Camera::getCam()->getFocal().y)/vs.z)+Camera::getCam()->getPrinc().y);
}
