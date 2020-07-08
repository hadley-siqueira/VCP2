#include <unistd.h>      /* pause() */

#include <xcb/xcb.h>

int main () {
  xcb_connection_t *c;
  xcb_screen_t     *screen;
  xcb_window_t      win;
  xcb_gcontext_t    gc;
  uint32_t          mask;
  uint32_t          values[5];

  xcb_point_t points[4] = { 10, 10, 15, 15, 20, 20, 25, 25 };

  /* Open the connection to the X server */
  c = xcb_connect (NULL, NULL);

  /* Get the first screen */
  screen = xcb_setup_roots_iterator (xcb_get_setup (c)).data;

  /* Ask for our window's Id */
  win = xcb_generate_id(c);
  gc = xcb_generate_id (c);


  /* Create the window */
  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  values[0] = screen->white_pixel;
  values[1] = XCB_EVENT_MASK_EXPOSURE;
  xcb_create_window (c,                             /* Connection          */
                     XCB_COPY_FROM_PARENT,          /* depth (same as root)*/
                     win,                           /* window Id           */
                     screen->root,                  /* parent window       */
                     0, 0,                          /* x, y                */
                     150, 150,                      /* width, height       */
                     10,                            /* border_width        */
                     XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                     screen->root_visual,           /* visual              */
                     mask, values);                      /* masks, not used yet */

  mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
  values[0] = screen->black_pixel;
  values[1] = screen->white_pixel;
  xcb_create_gc (c, gc, win, mask, values);
  xcb_poly_point(c, XCB_COORD_MODE_ORIGIN, win, gc, 4, points);

  /* Map the window on the screen */
  xcb_map_window (c, win);

  /* Make sure commands are sent before we pause, so window is shown */
  xcb_flush (c);

  pause ();    /* hold client until Ctrl-C */

  return 0;
}
