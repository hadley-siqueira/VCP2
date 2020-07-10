#include <stdlib.h>
#include <stdio.h>

#include <xcb/xcb.h>

void initPixels(uint8_t* pixels, int len) {
    int i;

    for (i = 0; i < len; i += 4) {
        pixels[i] = 0;
        pixels[i+1] = 0;
        pixels[i+2] = 255;
        pixels[i+3] = 125;
    }
}

/*
xcb_void_cookie_t
xcb_put_image (xcb_connection_t *c,
               uint8_t           format,
               xcb_drawable_t    drawable,
               xcb_gcontext_t    gc,
               uint16_t          width,
               uint16_t          height,
               int16_t           dst_x,
               int16_t           dst_y,
               uint8_t           left_pad,
               uint8_t           depth,
               uint32_t          data_len,
               const uint8_t    *data);

*/



int
main ()
{
  xcb_connection_t    *c;
  xcb_screen_t        *screen;
  xcb_drawable_t       win;
  xcb_gcontext_t       foreground;
  xcb_gcontext_t       fillpix;
  xcb_generic_event_t *e;
  xcb_pixmap_t         pixmap;
  uint32_t             mask = 0;
  uint32_t             values[2];
  uint8_t* pixels = malloc(250 * 250 * 7);
  xcb_void_cookie_t ck;
  xcb_generic_error_t* err;


  /* geometric objects */
  xcb_point_t          points[] = {
    {10, 10},
    {10, 20},
    {20, 10},
    {20, 20}};

  xcb_point_t          polyline[] = {
    {50, 10},
    { 5, 20},     /* rest of points are relative */
    {25,-20},
    {10, 10}};

  xcb_segment_t        segments[] = {
    {100, 10, 140, 30},
    {110, 25, 130, 60}};

  xcb_rectangle_t      rectangles[] = {
    { 10, 50, 40, 20},
    { 80, 50, 10, 40}};

  xcb_arc_t            arcs[] = {
    {10, 100, 60, 40, 0, 90 << 6},
    {90, 100, 55, 40, 0, 270 << 6}};

  initPixels(pixels, 250 * 250 * 4);
  /* Open the connection to the X server */
  c = xcb_connect (NULL, NULL);

  /* Get the first screen */
  screen = xcb_setup_roots_iterator (xcb_get_setup (c)).data;

  /* Create black (foreground) graphic context */
  win = screen->root;

  foreground = xcb_generate_id (c);
  mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = screen->black_pixel;
  values[1] = 0;//screen->black_pixel;
  xcb_create_gc (c, foreground, win, mask, values);

  fillpix = xcb_generate_id(c);
  mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = 0x00ff00;
  values[1] = screen->white_pixel;
  values[2] = 0;
  xcb_create_gc(c, fillpix, win, mask, values);

  /* Ask for our window's Id */
  win = xcb_generate_id(c);

  /* Create the window */
  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  values[0] = screen->white_pixel;
  values[1] = XCB_EVENT_MASK_EXPOSURE;
  xcb_create_window (c,                             /* Connection          */
                     XCB_COPY_FROM_PARENT,          /* depth               */
                     win,                           /* window Id           */
                     screen->root,                  /* parent window       */
                     0, 0,                          /* x, y                */
                     250, 250,                      /* width, height       */
                     10,                            /* border_width        */
                     XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                     screen->root_visual,           /* visual              */
                     mask, values);                 /* masks */

  /* Create pixmap */
  pixmap = xcb_generate_id(c);

  xcb_create_pixmap(c, screen->root_depth, pixmap, win, 250, 250);

  //xcb_poly_fill_rectangle(c, pixmap, fillpix, 1, (xcb_rectangle_t[]){{ 0, 0, 150, 150}});
  //ck = xcb_put_image(c, XCB_IMAGE_FORMAT_Z_PIXMAP, pixmap, fillpix, 100, 100, 0, 0, 0, screen->root_depth, 100 * 100 * 3, pixels);
  printf("depth: %i\n", screen->root_depth);

  /* Map the window on the screen */
  xcb_map_window (c, win);

  /* We flush the request */
  xcb_flush (c);

  while ((e = xcb_wait_for_event (c))) {
    switch (e->response_type & ~0x80) {
    case XCB_EXPOSE: {
      /* We draw the points */
      xcb_poly_point (c, XCB_COORD_MODE_ORIGIN, win, foreground, 4, points);

      /* We draw the polygonal line */
      xcb_poly_line (c, XCB_COORD_MODE_PREVIOUS, win, foreground, 4, polyline);

      /* We draw the segements */
      xcb_poly_segment (c, win, fillpix, 2, segments);

      /* We draw the rectangles */
      xcb_poly_rectangle (c, win, foreground, 2, rectangles);

      /* We draw the arcs */
      xcb_poly_arc (c, win, foreground, 2, arcs);

      /* Draw the pixmap */
      ck = xcb_put_image(c, XCB_IMAGE_FORMAT_Z_PIXMAP, pixmap, fillpix, 250, 250, 0, 0, 0, screen->root_depth, 250 * 250 * 4, pixels);
      xcb_copy_area(c, pixmap, win, fillpix, 0, 0, 0, 0, 250, 250);
      err = xcb_request_check(c, ck);

      if (err != NULL) {
          printf("response_type: %i\n", err->response_type);
          printf("error_code: %i\n", err->error_code);
      }

      /* We flush the request */
      xcb_flush (c);

      break;
    }
    case 0:
        err = (xcb_generic_error_t*) e;
      if (err != NULL) {
          printf("response_type: %i\n", err->response_type);
          printf("error_code: %i\n", err->error_code);
          printf("sequence: %i\n", err->sequence);
          printf("major_code: %i\n", err->major_code);
          printf("minor_code: %i\n", err->minor_code);
      }
        break;
    default: {
      /* Unknown event type, ignore it */
      break;
    }
    }
    /* Free the Generic Event */
    free (e);
  }

  return 0;
}
