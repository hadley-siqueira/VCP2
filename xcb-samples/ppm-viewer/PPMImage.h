#ifndef PPM_IMAGE_H
#define PPM_IMAGE_H

#include <vector>

#include "Pixel.h"

class PPMImage {
    public:
        void open(char* path);
        void save(char* path);
        int getWidth();
        int getHeight();
        char* get_as_bgrx();

    private:
        std::vector<std::vector<Pixel> > pixels;
        int width;
        int height;
};

#endif
