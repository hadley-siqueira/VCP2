#ifndef EVENT_H
#define EVENT_H

typedef enum EventKind {
    EVENT_NONE,
    EVENT_BUTTON_PRESS
} EventKind;

class Event {
    public:
        int get_kind();
        int get_x();
        int get_y();

    public:
        int kind;
        int x;
        int y;
        int rx;
        int ry;
};

#endif
