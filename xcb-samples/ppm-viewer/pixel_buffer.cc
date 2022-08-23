#include <stdlib.h>
#include "pixel_buffer.h"

PixelBuffer::PixelBuffer(int width, int height) {
    this->width = width;
    this->height = height;
    pixels = new uint8_t[width * height * 4];
}

PixelBuffer::~PixelBuffer() {
    delete pixels;
}

uint8_t* PixelBuffer::get_pixels() {
    return pixels;
}

void PixelBuffer::clear(int color) {
    uint8_t r = (color >> 16) & 0x0ff;
    uint8_t g = (color >> 8) & 0x0ff;
    uint8_t b = color & 0x0ff;

    for (int i = 0; i < width * height * 4; i += 4) {
        pixels[i] = b;
        pixels[i + 1] = g;
        pixels[i + 2] = r;
        pixels[i + 3] = 0;
    }
}

void PixelBuffer::set(int x, int y, int color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        uint8_t r = (color >> 16) & 0x0ff;
        uint8_t g = (color >> 8) & 0x0ff;
        uint8_t b = color & 0x0ff;
        int addr = y * width * 4 + x * 4;

        pixels[addr] = b;
        pixels[addr + 1] = g;
        pixels[addr + 2] = r;
        pixels[addr + 3] = 0;
    }
}

int PixelBuffer::get(int x, int y) {
    int color = 0;

    if (x >= 0 && x < width && y >= 0 && y < height) {
        int addr = y * width * 4 + x * 4;

        color = (color << 8) | (0x0ff & pixels[addr + 3]);
        color = (color << 8) | (0x0ff & pixels[addr + 2]);
        color = (color << 8) | (0x0ff & pixels[addr + 1]);
        color = (color << 8) | (0x0ff & pixels[addr + 0]);
    }

    return color;
}

void PixelBuffer::cross(int x, int y, int color) {
    clear(0xffffff);
    line(x, 0, x, height, color);
    line(0, y, width, y, color);
}

void PixelBuffer::line(int x0, int y0, int x1, int y1, int color) {
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
    int err = (dx>dy ? dx : -dy)/2, e2;

    for(;;) {
        set(x0,y0, color);
        if (x0==x1 && y0==y1) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

void PixelBuffer::resize(int width, int height) {
    uint8_t* npixels = new uint8_t[width * height * 4];

    this->width = width;
    this->height = height;

    delete pixels;
    pixels = npixels;
}

int PixelBuffer::get_width() {
    return width;
}

int PixelBuffer::get_height() {
    return height;
}

void PixelBuffer::blit(PixelBuffer* other, int x, int y, int w, int h) {
    for (int i = x; i < x + w; ++i) {
        for (int j = y; j < y + h; ++j) {
            set(i, j, other->get(i - x, j - y));
        }
    }
}
