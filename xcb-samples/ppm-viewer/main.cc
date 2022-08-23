#include <iostream>
#include "PPMImage.h"

#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>
#include "pixel_buffer.h"
#include "window.h"
#include "widget.h"

void open_and_show2(char* path) {
    xcb_connection_t* c;
    xcb_screen_t* screen;
    xcb_drawable_t win;
    xcb_gcontext_t gc;
    xcb_pixmap_t pixmap;
    xcb_generic_event_t* e;

    PixelBuffer pixbuf(800, 600);
    pixbuf.clear(0xffffff);
    pixbuf.line(0, 0, 850, 600, 0xff);

    uint8_t* pixels;
    int mask;
    int values[5];
    int done = 0;

    pixels = (uint8_t*) pixbuf.get_pixels();

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
                     pixbuf.get_width(), pixbuf.get_height(), /* width, height       */
                     1,                            /* border_width        */
                     XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                     screen->root_visual,           /* visual              */
                     mask, values);                 /* masks */


    pixmap = xcb_generate_id(c);
    xcb_create_pixmap(c, screen->root_depth, pixmap, win, pixbuf.get_width(), pixbuf.get_height());


    /* Map the window on the screen */
    xcb_map_window (c, win);

    /* We flush the request */
    xcb_flush (c);
    xcb_button_press_event_t* mouse;
    int x, y;
    bool draw_flag = false;
    int x0, y0, x1, y1;

    while (!done && (e = xcb_wait_for_event (c))) {
        switch (e->response_type & ~0x80) {
        case XCB_EXPOSE: 
            /* Draw the pixmap */
            xcb_put_image(c, XCB_IMAGE_FORMAT_Z_PIXMAP, 
                          pixmap, gc, pixbuf.get_width(), pixbuf.get_height(), 0, 0, 0, 
                          screen->root_depth, pixbuf.get_width() * pixbuf.get_height() * 4, pixels);

            xcb_copy_area(c, pixmap, win, gc, 0, 0, 0, 0, pixbuf.get_width(), pixbuf.get_height());

            /* We flush the request */
            xcb_flush (c);

            break;

        case XCB_KEY_PRESS:
            done = 1;
            break;

        case XCB_BUTTON_PRESS:
            mouse = (xcb_button_press_event_t*) e;
            x = mouse->event_x;
            y = mouse->event_y;
            if (draw_flag) {
                x1 = x;
                y1 = y;
                pixbuf.line(x0, y0, x1, y1, 0xff0000);
            } else {
                x0 = x;
                y0 = y;
            }
            std::cout << x << ", " << y << std::endl;

            pixbuf.set(x, y, 0xff0000);

            xcb_put_image(c, XCB_IMAGE_FORMAT_Z_PIXMAP, 
                          pixmap, gc, pixbuf.get_width(), pixbuf.get_height(), 0, 0, 0, 
                          screen->root_depth, pixbuf.get_width() * pixbuf.get_height() * 4, pixels);

            xcb_copy_area(c, pixmap, win, gc, 0, 0, 0, 0, pixbuf.get_width(), pixbuf.get_height());

            /* We flush the request */
            xcb_flush (c);
            draw_flag = !draw_flag;
            break;

        case XCB_MOTION_NOTIFY:
            mouse = (xcb_button_press_event_t*) e;
            std::cout << mouse->event_x << ", " << mouse->event_y << std::endl;
            x = mouse->event_x;
            y = mouse->event_y;

            if (draw_flag) 
            pixbuf.set(x, y, 0xff0000);
            pixbuf.cross(x, y, 0xff0000);

            xcb_put_image(c, XCB_IMAGE_FORMAT_Z_PIXMAP, 
                          pixmap, gc, pixbuf.get_width(), pixbuf.get_height(), 0, 0, 0, 
                          screen->root_depth, pixbuf.get_width() * pixbuf.get_height() * 4, pixels);

            xcb_copy_area(c, pixmap, win, gc, 0, 0, 0, 0, pixbuf.get_width(), pixbuf.get_height());

            /* We flush the request */
            xcb_flush (c);
            break;

        default:
            break;
        }

        free (e);
    }

}

void open_and_show(char* path) {
    xcb_connection_t* c;
    xcb_screen_t* screen;
    xcb_drawable_t win;
    xcb_gcontext_t gc;
    xcb_pixmap_t pixmap;
    xcb_generic_event_t* e;
    PPMImage ppm;
    uint8_t* pixels;
    int mask;
    int values[5];
    int done = 0;


    ppm.open(path);
    pixels = (uint8_t*) ppm.get_as_bgrx();

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
                     ppm.getWidth(), ppm.getHeight(), /* width, height       */
                     1,                            /* border_width        */
                     XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                     screen->root_visual,           /* visual              */
                     mask, values);                 /* masks */


    pixmap = xcb_generate_id(c);
    xcb_create_pixmap(c, screen->root_depth, pixmap, win, ppm.getWidth(), ppm.getHeight());


    /* Map the window on the screen */
    xcb_map_window (c, win);

    /* We flush the request */
    xcb_flush (c);
    xcb_button_press_event_t* mouse;
    int x, y;

    while (!done && (e = xcb_wait_for_event (c))) {
        switch (e->response_type & ~0x80) {
        case XCB_EXPOSE: 
            /* Draw the pixmap */
            xcb_put_image(c, XCB_IMAGE_FORMAT_Z_PIXMAP, 
                          pixmap, gc, ppm.getWidth(), ppm.getHeight(), 0, 0, 0, 
                          screen->root_depth, ppm.getWidth() * ppm.getHeight() * 4, pixels);

            xcb_copy_area(c, pixmap, win, gc, 0, 0, 0, 0, ppm.getWidth(), ppm.getHeight());

            /* We flush the request */
            xcb_flush (c);

            break;

        case XCB_KEY_PRESS:
            done = 1;
            break;

        case XCB_BUTTON_PRESS:
            mouse = (xcb_button_press_event_t*) e;
            x = mouse->event_x;
            y = mouse->event_y;
            std::cout << x << ", " << y << std::endl;

            pixels[x * ppm.getWidth() + y] = 0xff;
            /*pixels[y * ppm.getWidth() + x + 1] = 0;
            pixels[y * ppm.getWidth() + x + 2] = 0;
            pixels[y * ppm.getWidth() + x + 3] = 0;*/

            xcb_put_image(c, XCB_IMAGE_FORMAT_Z_PIXMAP, 
                          pixmap, gc, ppm.getWidth(), ppm.getHeight(), 0, 0, 0, 
                          screen->root_depth, ppm.getWidth() * ppm.getHeight() * 4, pixels);

            xcb_copy_area(c, pixmap, win, gc, 0, 0, 0, 0, ppm.getWidth(), ppm.getHeight());

            /* We flush the request */
            xcb_flush (c);
            break;

        case XCB_MOTION_NOTIFY:
            mouse = (xcb_button_press_event_t*) e;
            std::cout << mouse->event_x << ", " << mouse->event_y << std::endl;
            x = mouse->event_x;
            y = mouse->event_y;

            pixels[y * ppm.getWidth() + x] = 0xff;
            pixels[y * ppm.getWidth() + x + 1] = 0xf0;
            pixels[y * ppm.getWidth() + x + 2] = 0xf0;
            pixels[y * ppm.getWidth() + x + 3] = 0xf0;

            xcb_put_image(c, XCB_IMAGE_FORMAT_Z_PIXMAP, 
                          pixmap, gc, ppm.getWidth(), ppm.getHeight(), 0, 0, 0, 
                          screen->root_depth, ppm.getWidth() * ppm.getHeight() * 4, pixels);

            xcb_copy_area(c, pixmap, win, gc, 0, 0, 0, 0, ppm.getWidth(), ppm.getHeight());

            /* We flush the request */
            xcb_flush (c);
            break;

        default:
            break;
        }

        free (e);
    }

}

int test1(int argc, char* argv[]) {
    PPMImage img;

    img.open(argv[1]);
    img.save("copia.ppm");
    open_and_show2(argv[1]);

    return 0;
}

void clicked(Widget* widget, Event* event) {
    if (event->get_kind() == EVENT_BUTTON_PRESS) {
        std::cout << "fui clicado!!\n";
    }
}

void other_callback(Widget* widget, Event* event) {
    if (event->get_kind() == EVENT_BUTTON_PRESS) {
        std::cout << "eu tambem executo!\n";
    }
}

void mais_uma(Widget* widget, Event* ev) {
    if (ev->get_kind() == EVENT_BUTTON_PRESS) {
        std::cout << ev->get_x() << ", " << ev->get_y() << std::endl;
    }
}

int test2() {
    Window win(800, 600);

    Widget* wig = new Widget(10, 10, 70, 20);
    Widget* wig2 = new Widget(10, 150, 70, 20);

    win.add_widget(wig);
    win.add_widget(wig2);

    wig2->add_callback(clicked);
    wig2->add_callback(other_callback);
    wig->add_callback(mais_uma);
    wig2->add_callback(mais_uma);

    win.show();
    return 0;
}

int main(int argc, char* argv[]) {
    test2();

    return 0;
}
