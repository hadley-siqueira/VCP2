#include "PPMImage.h"

#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>

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
    values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
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

        default:
            break;
        }

        free (e);
    }

}

int main(int argc, char* argv[]) {
    PPMImage img;

    img.open(argv[1]);
    img.save("copia.ppm");
    open_and_show(argv[1]);

    return 0;
}
