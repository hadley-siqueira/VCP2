#ifndef VCP2_APPLICATION_H
#define VCP2_APPLICATION_H

#include <xcb/xcb.h>
#include <vector>

namespace vcp2 {
    class Application {
        private:
            xcb_connection_t* connection;
            xcb_screen_t* screen;
    };
}

#endif
