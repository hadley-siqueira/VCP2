#ifndef WIDGET_H
#define WIDGET_H

#include <string>
#include <vector>
#include "pixel_buffer.h"

class Window;
class Widget;
class Event;

typedef void (*callback_t)(Widget*, Event*);

class Widget {
    public:
        Widget();
        Widget(int x, int y, int width, int height);
        ~Widget();

    public:
        virtual void draw();
        void set_window(Window* window);
        int get_width();
        int get_height();
        bool is_inside(int x, int y);
        void exec_callbacks(Event* event=nullptr);
        void add_callback(callback_t cb);
        void process_event(Event* event);

    private:
        int x;
        int y;
        std::string id;
        PixelBuffer* buffer;
        Window* window;
        std::vector<callback_t> callbacks;
};

#endif
