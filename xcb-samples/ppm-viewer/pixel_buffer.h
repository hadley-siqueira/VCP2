#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H

#include <stdint.h>

class PixelBuffer {
    public:
        PixelBuffer(int width, int height);
        ~PixelBuffer();

    public:
        uint8_t* get_pixels();
        void set(int x, int y, int color);
        int get(int x, int y);
        void clear(int color);
        void resize(int width, int height);

        int get_width();
        int get_height();
        void blit(PixelBuffer* other, int x, int y, int w, int h);

        void line(int x0, int y0, int x1, int y1, int color);
        void cross(int x, int y, int color);

    private:
        int width;
        int height;
        uint8_t* pixels;
};

#endif
