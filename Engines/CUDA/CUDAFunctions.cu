/* Trying to implement the following algorithm using CUDA */
//for(int w = 0; w<frame->frame->getWidth(); w++){
//    for(int h = 0; h<frame->frame->getHeight(); h++){
//        for(int v = 0; v<verticesInPixel[w][h].size(); v++){

//            if(!(verticesInPixel[w][h][v]->assigned)){
//                // This is the case where the vertex has never been assigned a value before
//                if(verticesInPixel[w][h][v]->zDepth>1.05*depthPixels[w][h]){
//                    verticesInPixel[w][h][v]->TexCoords = glm::vec2(0,0);
//                } else {
//                    verticesInPixel[w][h][v]->TexCoords = glm::vec2(verticesInPixel[w][h][v]->TexCoords.x/640, verticesInPixel[w][h][v]->TexCoords.y/(480*totalFr)+((float)frNr/(float)totalFr));
//                    verticesInPixel[w][h][v]->assigned = true;
//                }
//            }
//        }
//    }
//}


/*
 * inputs: std::vector<std::vector<std::vector<*Vertex>>>
 * outputs: Modified
*/
#include <CUDAFunctions.h>
void findTexCoord(std::vector<std::vector<float> > *depthPixels,std::vector<std::vector<std::vector<Vertex *> > > *PixelsInImage, int* fr, int*frNr){
    int width = depthPixels->size();
    int height = depthPixels[0].size();
    findTexCoords<<<height,width>>>(depthPixels, PixelsInImage, fr, frNr);
}

__global__ void findTexCoords(std::vector<std::vector<float> > *depthPixels, std::vector<std::vector<std::vector<Vertex *> > > *verticesInPixel, int *fr, int *frNr){
    int w = blockIdx.x;
    int h = threadIdx.x;
    int FrameNumber = *fr; int TotalFrames = *frNr;
    for(int v = 0; v<(*verticesInPixel)[w][h].size(); v++){
        if(!((*verticesInPixel)[w][h][v]->assigned)){
            // This is the case where the vertex has never been assigned a value before
            if((*verticesInPixel)[w][h][v]->zDepth>1.05*(*depthPixels)[w][h]){
                (*verticesInPixel)[w][h][v]->TexCoords = glm::vec2(0,0);
            } else {
                (*verticesInPixel)[w][h][v]->TexCoords = glm::vec2((*verticesInPixel)[w][h][v]->TexCoords.x/640, (*verticesInPixel)[w][h][v]->TexCoords.y/(480*TotalFrames)+((float)FrameNumber/(float)TotalFrames));
                (*verticesInPixel)[w][h][v]->assigned = true;
            }
        }
    }

}
