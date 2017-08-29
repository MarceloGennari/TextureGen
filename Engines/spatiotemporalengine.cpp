#include "textureengine.h"


void TextureEngine::SaptiotemporalEngine::calcBlur(Image &im){
    /* This uses the method by Crete et al. (2007) to calculate the blurriness of an image
     * It is self contained as this is a no-reference Blur Estimation
     *
     * Also, we assume that the sharpness of the image is contained in its gray component (Crete et al., 2007)
     * So here we just calculated the blurrinness of the gray part of the image
     *
     * The steps in this calculation are:
     *
     * 1-> Blur the original frame F horizontally using 1/9[1 1 1 1 1 1 1 1 1] to obtain Bhor
     * 2-> Blur the original frame F vertically using 1/9[1 1 1 1 1 1 1 1 1]^T to obtain Bver
     * */

}
