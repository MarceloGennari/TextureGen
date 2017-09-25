#include "textureengine.h"
#include "camera.h"
#include "../model.h"
#include "objects.h"

#include <sstream>
#include <cmath>
#include <algorithm>
#include <iterator>

#ifndef CUDA_NOT_FOUND
    #include <CUDA/CUDAFunctions.h>
    #include <cuda_runtime_api.h>
    #include <cuda.h>
#endif

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

bool sortP(glm::vec2 a, glm::vec2 b){
    return a.x<b.x;
}

bool testFace(Vertex *v1, Vertex *v2, Vertex *v3){
    return(v1->TexCoords.x>0 && (v2->TexCoords.x==0 || v3->TexCoords.x == 0));
}

void TextureEngine::TextureMapGenEngine::getTextureCoords(std::vector<Vertex> &vs, std::vector<unsigned int> &ind, Frame *frame, int frNr, int totalFr){

        // This is the list of vertices per pixel
    std::vector<std::vector<std::vector<Vertex *> > > verticesInPixel;
    verticesInPixel.resize(frame->frame->getWidth());
    for(int k = 0; k<frame->frame->getWidth(); k++){
        verticesInPixel[k].resize(frame->frame->getHeight());
    }

    // This creates a vector of vertices that project to the pixel x,y
    std::string nr = SSTR(frame->frameNr-1);
    Camera::getCam()->positionCameraFrN(nr);

    // This is going to hold all of the faces that are inside this frame
    std::vector<int> faces;

    std::vector<std::vector<float> > depthPixels = zBuffering(vs, ind, verticesInPixel, frame, faces);


#ifndef CUDA_NOT_FOUND
    std::vector<std::vector<std::vector<Vertex *> > > * dev_verticesInPixel;
    std::vector<std::vector<float> > *dev_depthPixels;
    int *dev_fr; int *dev_frNr;

    cudaMalloc((void**) &dev_depthPixels, frame->frame->getHeight()*frame->frame->getWidth()*sizeof(float));
    // Chose 20 vertices per pixels as a placeholder for now. I'm going to change that to something more accurate in the future
    short nVperP = 20;
    cudaMalloc((void**) &dev_verticesInPixel, frame->frame->getHeight()*frame->frame->getWidth()*nVperP*sizeof(Vertex *));
    cudaMalloc((void**) &dev_fr, sizeof(int),);
    cudaMalloc((void**) &dev_frNr, sizeof(int));

    // Notice that depthPixels cannot be sent to Cuda directly because it is a std::vector
    // By using &depthPixels[0] we are sending the Contiguous array instead of the std::vector object
    // The same is valid for verticesInPixel
    cudaMemcpy(dev_depthPixels, &depthPixels[0], frame->frame->getHeight()*frame->frame->getWidth()*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_verticesInPixel, &verticesInPixel[0], frame->frame->getHeight()*frame->frame->getWidth()*nVperP*sizeof(Vertex *), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_fr, &frNr, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_frNr, &totalFr, sizeof(int), cudaMemcpyHostToDevice);
    findTexCoord(dev_depthPixels, dev_verticesInPixel, dev_fr, dev_frNr);

    cudaFree(dev_depthPixels);
    cudaFree(dev_verticesInPixel);
#endif
    // Now that we have all of the depths and all of the pixels, we can eliminate those vertices that are behind the pixel
    for(int w = 0; w<frame->frame->getWidth(); w++){
        for(int h = 0; h<frame->frame->getHeight(); h++){
            for(int v = 0; v<verticesInPixel[w][h].size(); v++){

                if(!(verticesInPixel[w][h][v]->assigned)){
                    // This is the case where the vertex has never been assigned a value before
                    if(verticesInPixel[w][h][v]->zDepth>1.05*depthPixels[w][h]){
                        verticesInPixel[w][h][v]->TexCoords = glm::vec2(0,0);
                    } else {
                        verticesInPixel[w][h][v]->TexCoords = glm::vec2(verticesInPixel[w][h][v]->TexCoords.x/640, verticesInPixel[w][h][v]->TexCoords.y/(480*totalFr)+((float)frNr/(float)totalFr));
                        verticesInPixel[w][h][v]->assigned = true;
                    }
                }
            }
        }
    }

    // Now there are some faces which have just one or two of the vertices assigned, which is not what we want
    // The next bit takes all of the faces that have less than three vertices assigned and:
    // Create a new vertex for each of the vertices assigned to that face with the same position of the initial vertex

    for(int k = 0; k<faces.size(); k++){
        unsigned int face = faces[k];
        unsigned int index1 = face*3;
        unsigned int index2 = face*3+1;
        unsigned int index3 = face*3+2;
        Vertex *v1 = &vs[ind[index1]];
        Vertex *v2 = &vs[ind[index2]];
        Vertex *v3 = &vs[ind[index3]];

        // Here we test whether all of the vertices has a value assigned for the texture coordinate
        /* The cases are:
         * v1, v2 and v3 are assigned
         * v1 is assigned but v2 and v3 are not
         * v2 is assigned but v1 and v3 are not
         * v3 is assigned but v1 and v2 are not
         * v1 and v2 are assigned but v3 is not
         * v2 and v3 are assigned but v1 is not
         * v1 and v3 are assigned but v2 is not
         * */

        if((v1->TexCoords.x>0 && v2->TexCoords.x>0 && v3->TexCoords.x>0) || (!(v1->assigned)&&!(v2->assigned)&&!(v3->assigned)))
            continue;

        // Case 1
        if(testFace(v1,v2,v3)){
            Vertex newV = *v1;
            newV.TexCoords = glm::vec2(0,0);
            newV.TexCoords2 = glm::vec2(0,0);
            newV.assigned = false;
            vs.push_back(newV);
            ind[index1] = vs.size()-1;
        }

        // Case 2
        if(testFace(v2,v1,v3)){
            Vertex newV = *v2;
            newV.TexCoords = glm::vec2(0,0);
            newV.TexCoords2 = glm::vec2(0,0);
            newV.assigned = false;
            vs.push_back(newV);
            ind[index2] = vs.size()-1;
        }

        // Case 3
        if(testFace(v3,v1,v2)){
            Vertex newV = *v3;
            newV.TexCoords = glm::vec2(0,0);
            newV.TexCoords2 = glm::vec2(0,0);
            newV.assigned = false;
            vs.push_back(newV);
            ind[index3] = vs.size()-1;
        }
    }
}

glm::vec2 TextureEngine::TextureMapGenEngine::project(glm::vec3 &vs){
    return glm::vec2(((vs.x*Camera::getCam()->getFocal().x)/vs.z)+Camera::getCam()->getPrinc().x,
                     ((vs.y*Camera::getCam()->getFocal().y)/vs.z)+Camera::getCam()->getPrinc().y);
}

std::vector<std::vector<float> > TextureEngine::TextureMapGenEngine::zBuffering(std::vector<Vertex> &vs, std::vector<unsigned int> &ind, std::vector<std::vector<std::vector<Vertex *> > > &verticesInPixel, Frame* frame, std::vector<int> &faces){

    /* The idea of the algorithm is:
     * Given the pose of the camera, the map and the projected points:
     * We calculate the z-Buffer value for each pixel and we give a list of vertices in each pixel
     * */

    // This is the z-Buffer and will store the lowest value of the depth for each pixel in the image
    std::vector<std::vector<float> > depthPixels;
    depthPixels.resize(frame->frame->getWidth());
    for(int k = 0; k<frame->frame->getWidth(); k++){
        depthPixels[k].resize(frame->frame->getHeight());
    }

    // This will contain a pointer to all of the vertices that fall within the contraint above
    std::vector<unsigned int> verticesInsideImage;

    // Projecting the Vertices to the RGB coordinate
    for(int k =0; k<vs.size(); k++){
        glm::vec3 posCamCoord = glm::vec3(Camera::getCam()->getView()*glm::vec4(vs[k].Pos, 1.0f));
        glm::vec2 position = project(posCamCoord);
        glm::vec2 pixel = glm::vec2(round(position.x), round(position.y));

        // Those two are hacks because the camera pose is not right or I don't know exactly how to use the poses
        pixel.y = pixel.y +5;
        pixel.x = pixel.x -65;
        position.y = position.y +5;
        position.x = position.x -65;

        // This is also limiting distance just in case things get strected because of the distance + the further the point the less accurate the projection will be
        if(pixel.x<=640&&pixel.x>0&&pixel.y<=480&&pixel.y>0 && std::abs(posCamCoord.z) < 1.5f){

            position.x = 641-position.x;
            pixel.x = 641-pixel.x;

            verticesInsideImage.push_back(k);

            vs[k].zDepth = std::abs(posCamCoord.z);
            depthPixels[pixel.x-1][pixel.y-1] = std::abs(posCamCoord.z);
            verticesInPixel[pixel.x-1][pixel.y-1].push_back(&vs[k]);

            vs[k].TexCoords2.x = position.x;
            vs[k].TexCoords2.y = position.y;

            if(vs[k].TexCoords.x>0) // This accounts for the case where a coordinate has already been assigned to that vertex
                continue;

            vs[k].TexCoords.x = position.x;
            vs[k].TexCoords.y = position.y;
        }
    }

    /* Now that we have all of the vertices that makes up the image, we will need to get all of the faces.
     * */
    for(int k = 0; k<verticesInsideImage.size(); k++){
        // Remember that each vertex has a vector of faces that it belongs to
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
        Face.push_back(vs[ind[faces[f]*3]].TexCoords2);
        Face.push_back(vs[ind[faces[f]*3+1]].TexCoords2);
        Face.push_back(vs[ind[faces[f]*3+2]].TexCoords2);

        std::sort(Face.begin(), Face.end(), sortP);

        if((Face[0].x==0&&Face[0].y==0)||(Face[1].x==0&&Face[1].y==0)||(Face[2].x==0&&Face[2].y==0))
            continue; // This will prevent vertices that are not in the image

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

        for(int h =0; h<pixelsInTriangle.size(); h++){
            float *currentDepth = &depthPixels[pixelsInTriangle[h].x-1][pixelsInTriangle[h].y-1];
            if(*currentDepth!=0 && std::abs(*currentDepth)>std::abs(vs[ind[faces[f]*3]].zDepth)){
                *currentDepth = std::abs(vs[ind[faces[f]*3]].zDepth);
            }
        }
    }


    return depthPixels;
}

