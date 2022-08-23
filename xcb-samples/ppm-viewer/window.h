#ifndef WINDOW_H
#define WINDOW_H

#include <vector>
#include <xcb/xcb.h>
#include "widget.h"
#include "pixel_buffer.h"
#include "event.h"

class Window {
    public:
        Window(int width, int height);
        ~Window();

    public:
        void add_widget(Widget* widget);
        void draw();
        PixelBuffer* get_buffer();
        int get_width();
        int get_height();
        void show();

    private:
        void handle_xcb();
        void send_event(Event* event);

    private:
        std::vector<Widget*> widgets;
        PixelBuffer* buffer;

    // XCB stuff
    private:
        xcb_connection_t* c;
        xcb_screen_t* screen;
        xcb_drawable_t win;
        xcb_gcontext_t gc;
        xcb_pixmap_t pixmap;
        xcb_generic_event_t* e;
        int mask;
        int values[5];
        int done = 0;
};

#endif
