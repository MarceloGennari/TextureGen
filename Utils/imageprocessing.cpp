#include "objects.h"
#include <algorithm>

unsigned char * ImageProcessing::absVerDiff(Image &im){
    /* I am assuming here that the image is grayscale of unsigned char
     * */
    int width = im.getWidth();
    int height = im.getHeight();
    unsigned char * D_F = (unsigned char *) malloc(sizeof(unsigned char)*width*height);

    int x, y;
    x = 0;
    for(int w = 0; w<width*height; w++){
        y = floor(w/width);
        y = y+1;
        if(w%width == 0){
            x = 1;
        }
        if(x==1){
            im.getPixel(x,y, D_F[w]);
        } else {
            unsigned char p1, p2;
            im.getPixel(x,y,p1);
            im.getPixel(x-1,y,p2);
            D_F[w] = abs(p1-p2);
        }
        x++;
    }

    return D_F;
}

unsigned char * ImageProcessing::absHorDiff(Image &im){
    /* I am assuming here that the image is grayscale of unsigned char
     * */
    int width = im.getWidth();
    int height = im.getHeight();
    unsigned char * D_F = (unsigned char *) malloc(sizeof(unsigned char)*width*height);

    int x, y;
    x = 0;
    for(int w = 0; w<width*height; w++){
        y = floor(w/width);
        y = y+1;
        if(w%width == 0){
            x = 1;
        }
        if(y==1){
            im.getPixel(x,y, D_F[w]);
        } else {
            unsigned char p1, p2;
            im.getPixel(x,y,p1);
            im.getPixel(x,y-1,p2);
            D_F[w] = abs(p1-p2);
        }
        x++;
    }

    return D_F;
}

float ImageProcessing::calcBlur(Image &im){
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
     * 3-> Get Absolute Differences D_Fver, D_Fhor, D_Bver, D_Bhor
     * 4-> Calculate D_Vver and D_Vhor
     * */

    int width = im.getWidth();
    int height = im.getHeight();

    Image Bhor(width, height, ImageProcessing::ImageFilters::horLPF(im), "gray");
    Image Bver(width, height, ImageProcessing::ImageFilters::verLPF(im), "gray");
    unsigned char *D_Fver = ImageProcessing::absVerDiff(im);
    unsigned char *D_Fhor = ImageProcessing::absHorDiff(im);
    unsigned char *D_Bver = ImageProcessing::absVerDiff(Bver);
    unsigned char *D_Bhor = ImageProcessing::absHorDiff(Bhor);

    unsigned char * D_Vver = (unsigned char *)malloc(sizeof(unsigned char)*width*height);
    unsigned char * D_Vhor = (unsigned char *)malloc(sizeof(unsigned char)*width*height);

    for(int k = 0; k<width*height; k++){
        D_Vver[k] = std::max(0,(int)(D_Fver[k]-D_Bver[k]));
        D_Vhor[k] = std::max(0,(int)(D_Fhor[k]-D_Bhor[k]));
    }

    int s_Fver =0, s_Fhor =0, s_Vver=0, s_Vhor=0;

    for(int k = 0; k<width*height; k ++){
        s_Fver = s_Fver + (int)D_Fver[k];
        s_Fhor = s_Fhor + (int)D_Fhor[k];
        s_Vver = s_Vver + (int)D_Vver[k];
        s_Vhor = s_Vhor + (int)D_Vhor[k];
    }

    float b_Fver, b_Fhor;

    b_Fver = (s_Fver-s_Vver)/(float)s_Fver;
    b_Fhor = (s_Fhor-s_Vhor)/(float)s_Fhor;

    return std::max(b_Fver, b_Fhor);
}
