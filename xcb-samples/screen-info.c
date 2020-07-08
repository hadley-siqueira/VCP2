#include <stdio.h>

#include <xcb/xcb.h>

int
main ()
{
  xcb_connection_t     *c;
  xcb_screen_t         *screen;
  int                   screen_nbr;
  xcb_screen_iterator_t iter;

  /* Open the connection to the X server. Use the DISPLAY environment variable */
  c = xcb_connect (NULL, &screen_nbr);

  /* Get the screen #screen_nbr */
  iter = xcb_setup_roots_iterator (xcb_get_setup (c));
  for (; iter.rem; --screen_nbr, xcb_screen_next (&iter))
    if (screen_nbr == 0) {
      screen = iter.data;
      break;
    }

  printf ("\n");
  printf ("Informations of screen %ld:\n", screen->root);
  printf ("  width.........: %d\n", screen->width_in_pixels);
  printf ("  height........: %d\n", screen->height_in_pixels);
  printf ("  white pixel...: %ld\n", screen->white_pixel);
  printf ("  black pixel...: %ld\n", screen->black_pixel);
  printf ("\n");

  return 0;
}
