#include <iostream>
#include "window.h"

Window::Window(int width, int height) {
    buffer = new PixelBuffer(width, height);
}

Window::~Window() {
    for (int i = 0; i < widgets.size(); ++i) {
        delete widgets[i];
    }

    delete buffer;
}

void Window::send_event(Event* event) {
    for (int i = 0; i < widgets.size(); ++i) {
        widgets[i]->process_event(event);
    }
}

void Window::add_widget(Widget* widget) {
    widgets.push_back(widget);
    widget->set_window(this);
}

void Window::draw() {
    buffer->clear(0xffffff);

    for (int i = 0; i < widgets.size(); ++i) {
        widgets[i]->draw();
    }

    /* Draw the pixmap */
    xcb_put_image(c, XCB_IMAGE_FORMAT_Z_PIXMAP, 
                  pixmap, gc, get_width(), get_height(), 0, 0, 0, 
                  screen->root_depth, get_width() * get_height() * 4, buffer->get_pixels());

    xcb_copy_area(c, pixmap, win, gc, 0, 0, 0, 0, get_width(), get_height());

    /* We flush the request */
    xcb_flush (c);
}

PixelBuffer* Window::get_buffer() {
    return buffer;
}

int Window::get_width() {
    return buffer->get_width();
}

int Window::get_height() {
    return buffer->get_height();
}

void Window::show() {
    handle_xcb();
}

void Window::handle_xcb() {
    c = xcb_connect(NULL, NULL);
    screen = xcb_setup_roots_iterator (xcb_get_setup (c)).data;
    win = screen->root;

    gc = xcb_generate_id (c);
    mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    values[0] = screen->black_pixel;
    values[1] = 0;//screen->black_pixel;
    xcb_create_gc (c, gc, win, mask, values);

    win = xcb_generate_id(c);

    /* Create the window */
    mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    values[0] = screen->white_pixel;
    values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_POINTER_MOTION;
    xcb_create_window (c,                             /* Connection          */
                     XCB_COPY_FROM_PARENT,          /* depth               */
                     win,                           /* window Id           */
                     screen->root,                  /* parent window       */
                     0, 0,                          /* x, y                */
                     get_width(), get_height(), /* width, height       */
                     1,                            /* border_width        */
                     XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                     screen->root_visual,           /* visual              */
                     mask, values);                 /* masks */


    pixmap = xcb_generate_id(c);
    xcb_create_pixmap(c, screen->root_depth, pixmap, win, get_width(), get_height());


    /* Map the window on the screen */
    xcb_map_window (c, win);

    /* We flush the request */
    xcb_flush (c);

    Event event;
    xcb_button_press_event_t* mouse;
    while (!done && (e = xcb_wait_for_event (c))) {
        switch (e->response_type & ~0x80) {
        case XCB_EXPOSE: 
            break;

        case XCB_KEY_PRESS:
            break;

        case XCB_BUTTON_PRESS:
            mouse = (xcb_button_press_event_t*) e;
            event.x = mouse->event_x;
            event.y = mouse->event_y;
            event.kind = EVENT_BUTTON_PRESS;
            break;

        case XCB_MOTION_NOTIFY:
            break;

        default:
            break;
        }

        send_event(&event);
        draw();
        event.kind = EVENT_NONE;
        free (e);
    }

}

