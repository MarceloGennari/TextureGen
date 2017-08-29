#include "objects.h"

using namespace ImageProcessing;

unsigned char * ImageFilters::horLPF(Image &im){
    int width = im.getWidth();
    int height = im.getHeight();
    /* Assuming that the filter will be 1/9[1 1 1 1 1 1 1 1 1]
     * So the new image will "lose" the blur in the edges
     * */
    unsigned char *Bhor = NULL;
    Bhor = (unsigned char *) malloc(width*height*sizeof(unsigned char));
    int x, y;

    x = 0;
    for(int w = 0; w<width*height; w++){
        y = floor(w/width);
        y = y+1;
        if(w%width == 0){
            x = 1;
        }
        if(x<=4 || x>width-4){
            im.getPixel(x,y, Bhor[w]);
        } else {
            unsigned char x_4, x_3, x_2, x_1, x0, x1, x2, x3, x4;
            im.getPixel(x-4, y, x_4);
            im.getPixel(x-3, y, x_3);
            im.getPixel(x-2, y, x_2);
            im.getPixel(x-1, y, x_1);
            im.getPixel(x, y, x0);
            im.getPixel(x+1, y, x1);
            im.getPixel(x+2, y, x2);
            im.getPixel(x+3, y, x3);
            im.getPixel(x+4, y, x4);
            int sum = x_4 + x_3 + x_2 + x_1 + x0 + x1 + x2 + x3 + x4;
            unsigned char s = sum/9;
            Bhor[w] = s;
        }
        x++;
    }

    return Bhor;
}

unsigned char * ImageFilters::verLPF(Image &im){
    int width = im.getWidth();
    int height = im.getHeight();
    /* Assuming that the filter will be 1/9[1 1 1 1 1 1 1 1 1]
     * So the new image will "lose" the blur in the edges
     * */
    unsigned char *Bhor = NULL;
    Bhor = (unsigned char *) malloc(width*height*sizeof(unsigned char));
    int x, y;

    x = 0;
    for(int w = 0; w<width*height; w++){
        y = floor(w/width);
        y = y+1;
        if(w%width == 0){
            x = 1;
        }
        if(y<=4 || y>height-4){
            im.getPixel(x,y, Bhor[w]);
        } else {
            unsigned char y_4, y_3, y_2, y_1, y0, y1, y2, y3, y4;
            im.getPixel(x, y-4, y_4);
            im.getPixel(x, y-3, y_3);
            im.getPixel(x, y-2, y_2);
            im.getPixel(x, y-1, y_1);
            im.getPixel(x, y, y0);
            im.getPixel(x, y+1, y1);
            im.getPixel(x, y+2, y2);
            im.getPixel(x, y+3, y3);
            im.getPixel(x, y+4, y4);
            int sum = y_4 + y_3 + y_2 + y_1 + y0 + y1 + y2 + y3 + y4;
            unsigned char s = sum/9;
            Bhor[w] = s;
        }
        x++;
    }

    return Bhor;
}
