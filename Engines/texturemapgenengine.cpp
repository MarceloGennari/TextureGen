#include "textureengine.h"
#include "camera.h"
#include "../model.h"
#include "objects.h"

#include <sstream>
#include <cmath>
#include <algorithm>
#include <iterator>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

bool sortP(glm::vec2 a, glm::vec2 b){
    return a.x<b.x;
}

bool isInsideTriangle(glm::vec2 TexCoord, std::vector<glm::vec2> Face){
    /* This just tests whether the point TexCoord is inside the Face (which is a vector of three points in the image)
     * This uses the method of Same Side Technique
     * */
    glm::vec2 p1 = Face[0];
    glm::vec2 p2 = Face[1];
    glm::vec2 p3 = Face[2];


}

void TextureEngine::TextureMapGenEngine::getTextureCoords(std::vector<Vertex> &vs, std::vector<unsigned int> &ind, Frame *frame){

    // This will contain a pointer to all of the vertices that fall within the contraint above
    std::vector<unsigned int> verticesInsideImage;
    std::vector<int> faces;

    // This is the z-Buffer and will store the lowest value of the depth for each pixel in the image
    std::vector<std::vector<float> > depthPixels;
    depthPixels.resize(frame->frame->getWidth());
    for(int k = 0; k<frame->frame->getWidth(); k++){
        depthPixels[k].resize(frame->frame->getHeight());
    }

    // This creates a vector of vertices that project to the pixel x,y
    std::string nr = SSTR(frame->frameNr-1);
    Camera::getCam()->positionCameraFrN(nr);

    // Projecting the Vertices to the RGB coordinate
    for(int k =0; k<vs.size(); k++){
        glm::vec3 posCamCoord = glm::vec3(Camera::getCam()->getView()*glm::vec4(vs[k].Pos, 1.0f));
//        glm::vec4 posCamCoord2 = Camera::getCam()->getKMat()*posDepthCam;
//        glm::vec3 posCamCoord = glm::vec3(posCamCoord2.x/posCamCoord2.w, posCamCoord2.y/posCamCoord2.w, posCamCoord2.z/posCamCoord2.w);
        glm::vec2 position = project(posCamCoord);
        glm::vec2 pixel = glm::vec2(round(position.x), round(position.y));

        pixel.y = pixel.y +5;
        pixel.x = pixel.x -70;

        position.y = position.y +5;
        position.x = position.x -70;

        if(pixel.x<=640&&pixel.x>0&&pixel.y<=480&&pixel.y>0){

            position.x = 641-position.x;
            pixel.x = 641-pixel.x;

            verticesInsideImage.push_back(k);

            vs[k].zDepth = std::abs(posCamCoord.z);
            depthPixels[pixel.x-1][pixel.y-1] = posCamCoord.z;
            vs[k].TexCoords.x = position.x;
            vs[k].TexCoords.y = position.y;
        }
    }

    /* Now that we have all of the vertices that makes up the image, we will need to get all of the faces.
     * */
    for(int k = 0; k<verticesInsideImage.size(); k++){
        faces.insert(faces.end(), vs[verticesInsideImage[k]].faces.begin(), vs[verticesInsideImage[k]].faces.end());
    }

    std::sort(faces.begin(), faces.end());
    faces.erase(std::unique(faces.begin(), faces.end()), faces.end());

    /* Now that we have all of the vertices and all of the faces that are within that range, we can do some z-Buffering to eliminate
     * the vertices that should not be visible
     * Now we are going to start to populate the zBuffer
     *
     * The algorithm is:
     * For each face, assign all pixels "inside" it with the "biggest" value
     * */
    for(int f = 0; f<faces.size(); f++){
        // Find the pixels that are inside this triangle by using the triangle rasterization algorithm:
        // https://stackoverflow.com/questions/8957028/getting-a-list-of-locations-within-a-triangle-in-the-form-of-x-y-positions
        std::vector<glm::vec2> pixelsInTriangle;
        std::vector<glm::vec2> Face;
        Face.push_back(vs[ind[f*3]].TexCoords);
        Face.push_back(vs[ind[f*3+1]].TexCoords);
        Face.push_back(vs[ind[f*3+2]].TexCoords);

        std::sort(Face.begin(), Face.end(), sortP);

        if((Face[0].x==0&&Face[0].y==0)||(Face[1].x==0&&Face[1].y==0)||(Face[2].x==0&&Face[2].y==0))
            continue; // This will prevent vertices that are not in the image

        if(Face[1].x-Face[0].x>3&& Face[2].x-Face[1].x>3)
            std::cout<< "success" << std::endl;

        float A1 = (Face[0].y - Face[1].y)/(Face[0].x-Face[1].x);
        float B1 = Face[0].y-A1*Face[0].x;

        float A2 = (Face[0].y - Face[2].y)/(Face[0].x-Face[2].x);
        float B2 = Face[0].y-A2*Face[0].x;


        int startX = floor(Face[0].x+1);
        int endX = floor(Face[1].x);
        float y_top, y_bottom;

        for(int k = startX; k<=endX; k++){
            float y1 = A1*k+B1;
            float y2 = A2*k+B2;

            if(y1>y2){
                y_top = y1;
                y_bottom = y2;
            } else {
                y_top = y2;
                y_bottom = y1;
            }


            int y_b = floor(y_bottom+1);
            int y_t = floor(y_top);

            for(int r = y_b; r<=y_t; r++){
                pixelsInTriangle.push_back(glm::vec2(k, r));
            }
        }

        float A3 = (Face[1].y - Face[2].y)/(Face[1].x-Face[2].x);
        float B3 = Face[1].y-A3*Face[1].x;

        startX = floor(Face[1].x+1);
        endX = floor(Face[2].x);

        for(int k = startX; k<=endX; k++){
            float y1 = A3*k+B3;
            float y2 = A2*k+B2;

            if(y1>y2){
                y_top = y1;
                y_bottom = y2;
            } else {
                y_top = y2;
                y_bottom = y1;
            }

            int y_b = floor(y_bottom+1);
            int y_t = floor(y_top);

            for(int r = y_b; r<=y_t; r++){
                pixelsInTriangle.push_back(glm::vec2(k, r));
            }
        }

        if(pixelsInTriangle.size()>5){
            continue;
        }

    }






    // ZBuffer 1: It doesn't work since there are pixels that are assigned to vertices in the back
    // We need to use planes instead of vertices to check for vertices that are in the back
    //    for(int w = 0; w<frame->frame->getWidth(); w++){
    //        for(int h = 0; h<frame->frame->getHeight(); h++){
    //            std::sort(verticesInPixel[w][h].begin(), verticesInPixel[w][h].end(), sortVertices);
    //            if(!(verticesInPixel[w][h].empty())){
    //                float inDepth = verticesInPixel[w][h][0]->zDepth;
    //                for(int k = 1; k<verticesInPixel[w][h].size(); k++){
    //                    if(verticesInPixel[w][h][k]->zDepth-0.05f>inDepth){
    //                        while(k<verticesInPixel[w][h].size()){
    //                            verticesInPixel[w][h][k]->TexCoords = glm::vec2(0,0);
    //                            k++;
    //                        }
    //                        continue;
    //                    } else {
    //                        inDepth = verticesInPixel[w][h][k]->zDepth;
    //                    }
    //                }
    //            }
    //        }
    //    }
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

