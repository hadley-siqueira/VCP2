#include <iostream>

#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>

xcb_screen_t *screen;
xcb_window_t win;
xcb_gcontext_t gc;
xcb_pixmap_t backing_pixmap;
int w = 800;
int h = 600;

struct pixel_buffer {
    int w;
    int h;
    int bytes_per_row;
    void *pixels;
} buffer;

void init_buffer() {
    int i;
    uint8_t* p;

    buffer.pixels = malloc(sizeof(int) * 1000 * 1000);
    buffer.w = 800;
    buffer.h = 600;
    buffer.bytes_per_row = 2400;

    p = (uint8_t*) buffer.pixels;

    for (i = 0; i < 1000 * 1000 * sizeof(int); ++i) {
        p[i] = i % 256;
    }
}

void init_gc(xcb_connection_t *c) {
    uint32_t values[] = {screen->black_pixel, screen->white_pixel};
    gc = xcb_generate_id(c);
    xcb_create_gc(c, gc, backing_pixmap, XCB_GC_FOREGROUND | XCB_GC_BACKGROUND, values);
}

void init_backing_pixmap(xcb_connection_t *c) {
    backing_pixmap = xcb_generate_id(c);
    xcb_create_pixmap(c, screen->root_depth, backing_pixmap, win, w, h);

    init_gc(c);
}

/*
 * Note that without copying the data to a temporary buffer we can send updates
 * only as a horizontal stripes.
 */
void update_backing_pixmap(xcb_connection_t *c, int x, int y, int w, int h) {
    /* Send image data to X server */
    xcb_put_image(c, XCB_IMAGE_FORMAT_Z_PIXMAP, backing_pixmap,
                  gc, w, h, 0, y, 0,
                  screen->root_depth, buffer.bytes_per_row * h,
                  (uint8_t*) buffer.pixels +
                  buffer.bytes_per_row * y);

    /* Copy updated data to window */
    xcb_copy_area(c, backing_pixmap, win, gc, x, y, x, y, w, h);
    xcb_flush(c);
}

xcb_visualtype_t *visual_by_id(xcb_screen_t *screen) {
    xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(screen);
    int depth;
    xcb_visualtype_t *visual = NULL;

    for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
        xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(depth_iter.data);

        depth = depth_iter.data->depth;

        for (; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
            if (screen->root_visual == visual_iter.data->visual_id) {
                visual = visual_iter.data;
                goto found;
            }
        }
    }

    printf("Failed to match visual id\n");
    return NULL;
found:
    if (visual->_class != XCB_VISUAL_CLASS_TRUE_COLOR &&
        visual->_class != XCB_VISUAL_CLASS_DIRECT_COLOR) {
        printf("Unsupported visual\n");
        return NULL;
    }

    printf("depth %i, R mask %x G mask %x B mask %x\n",
           depth, visual->red_mask, visual->green_mask, visual->blue_mask);

    return visual;
}


int bpp_by_depth(xcb_connection_t *c, int depth) {
    const xcb_setup_t *setup = xcb_get_setup(c);
    xcb_format_iterator_t fmt_iter = xcb_setup_pixmap_formats_iterator(setup);

    for (; fmt_iter.rem; xcb_format_next(&fmt_iter)) {
        std::cout << (int) fmt_iter.data->depth << '\n';
        if (fmt_iter.data->depth == depth)
            return fmt_iter.data->bits_per_pixel;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    xcb_connection_t* conn;
    xcb_generic_event_t *e;
    int done = 0;
    uint32_t mask;
    uint32_t values[2];


    conn = xcb_connect(NULL, NULL);

    /* check if connection was successful */
    if (xcb_connection_has_error(conn)) {
        std::cout << "Cannot open display\n";
        exit(1);
    }

    bpp_by_depth(conn, 32);

    /* get the first screen */
    screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
    visual_by_id(screen);

/* Ask for our window's Id */
  win = xcb_generate_id(conn);
    mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    values[0] = screen->white_pixel;
    values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;

  /* Create the window */
  xcb_create_window (conn,                             /* Connection          */
                     XCB_COPY_FROM_PARENT,          /* depth (same as root)*/
                     win,                           /* window Id           */
                     screen->root,                  /* parent window       */
                     0, 0,                          /* x, y                */
                     800, 600,                      /* width, height       */
                     10,                            /* border_width        */
                     XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                     screen->root_visual,           /* visual              */
                     mask, values);                      /* masks, not used yet */

  /* Map the window on the screen */
  xcb_map_window (conn, win);

  /* Make sure commands are sent before we pause, so window is shown */
  xcb_flush (conn);

    init_gc(conn);
    init_backing_pixmap(conn);
    init_buffer();

    std::cout << "loop\n";

    /* event loop */
    while (!done && (e = xcb_wait_for_event(conn))) {
        switch (e->response_type & ~0x80) {
        case XCB_EXPOSE:    /* draw or redraw the window */
            //xcb_poly_fill_rectangle(conn, win, gc,  1, &r);
            update_backing_pixmap(conn, 0, 0, 800, 600);
            xcb_flush(conn);
            break;

        case XCB_KEY_PRESS:  /* exit on key press */
            done = 1;
            update_backing_pixmap(conn, 0, 0, 800, 600);
            xcb_flush(conn);
            break;
        }

        free(e);
    }

    /* close connection to server */
    xcb_disconnect(conn);
    return 0;
}
