#ifndef PIXEL_H
#define PIXEL_H

class Pixel {
    public:
        Pixel();
        Pixel(char r, char g, char b);

    public:
        void set(char r, char g, char b);
        int red();
        int green();
        int blue();

    private:
        char r;
        char g;
        char b;
};

#endif
