#include "Pixel.h"

Pixel::Pixel() {
    this->r = 0;
    this->g = 0;
    this->b = 0;
}

Pixel::Pixel(char r, char g, char b) {
    this->r = r;
    this->g = g;
    this->b = b;
}


void Pixel::set(char r, char g, char b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

int Pixel::red() {
    return (unsigned char) r;
}

int Pixel::green() {
    return (unsigned char) g;
}

int Pixel::blue() {
    return (unsigned char) b;
}
