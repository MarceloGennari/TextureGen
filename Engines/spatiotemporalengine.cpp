#include "textureengine.h"
#include <algorithm>
#include <sstream>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

std::vector<Frame *> TextureEngine::SaptiotemporalEngine::temporalSampling(int sigMax, int sigMin, int nrFrames){
    /*  This algorithm will work like:
     *  1-> Calculate the Blurrinness of all of the frames
     *  2-> Check the blurrinness of the next sigMax frames and choose the one with the smallest blurriness
     *  3-> Skip sigMin frames from the last selected
     *  4-> Iterate from 2
     * */

    std::vector<Frame *> frames;
    std::string root = "/home/marcelo/TextureGen/Teddy/Frames/";
    for(int k = 0; k < nrFrames; k++){
        std::string st, st2;

        if(k<10){
            st = root + "000" + SSTR(k) + ".ppm";
        } else if(k<100){
            st = root + "00" + SSTR(k) + ".ppm";
        } else if(k<1000){
            st = root + "0" + SSTR(k) + ".ppm";
        }

        if(k<10){
            st2 = root + "000" + SSTR(k) + ".pgm";
        } else if(k<100){
            st2 = root + "00" + SSTR(k) + ".pgm";
        } else if(k<1000){
            st2 = root + "0" + SSTR(k) + ".pgm";
        }

        Image *im = new Image(st, "ppm");
        float blur = ImageProcessing::calcBlur(*im);
        Frame *f = new Frame;
        f->blur = blur;
        f->frameNr = k;
        f->frame = im;
        frames.push_back(f);
    }

    std::vector<Frame *> framesSelected;
    bool iterate = true;
    int current = 0;
    while(iterate){
        int rangeEnd = std::min(current + sigMax, nrFrames);
        std::sort(frames.begin()+current, frames.begin() + rangeEnd, less_than_blur());

        framesSelected.push_back(frames[current]);

        std::sort(frames.begin()+current, frames.begin() + rangeEnd, less_than_Nr());
        current = framesSelected[framesSelected.size()-1]->frameNr + sigMin;
        if(rangeEnd == nrFrames)
            iterate = false;
    }

    return framesSelected;
}
