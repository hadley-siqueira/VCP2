#include <iostream>
#include <fstream>

#include "PPMImage.h"

void PPMImage::open(char* path) {
    std::ifstream f;
    std::string str;
    int tmp;
    int r;
    int g;
    int b;

    f.open(path);

    f >> str; // reads P3 header
    f >> width;
    f >> height;
    f >> tmp; // reads the channel max value

    pixels.resize(height);

    for (int i = 0; i < height; ++i) {
        pixels[i].resize(width);
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            f >> r >> g >> b;
            pixels[i][j].set(r, g, b);
        }
    }

    f.close();
}

void PPMImage::save(char* path) {
    std::ofstream f;

    f.open(path);

    f << "P3\n";
    f << width << "\n";
    f << height << "\n";
    f << 255 << "\n";

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            f << pixels[i][j].red() << ' ';
            f << pixels[i][j].green() << ' ';
            f << pixels[i][j].blue() << '\n';
        }
    }

    f.close();
}

int PPMImage::getWidth() {
    return width;
}

int PPMImage::getHeight() {
    return height;
}

char* PPMImage::get_as_bgrx() {
    int k = 0;
    uint8_t* pix = new uint8_t[width * height * 4];

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            pix[k++] = pixels[i][j].blue();
            pix[k++] = pixels[i][j].green();
            pix[k++] = pixels[i][j].red();
            pix[k++] = 0;
        }
    }

    return (char*) pix;
}


