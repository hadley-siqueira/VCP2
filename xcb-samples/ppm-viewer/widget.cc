#include "widget.h"
#include "window.h"


Widget::Widget() {
    x = 0;
    y = 0;
    buffer = nullptr;
}

Widget::Widget(int x, int y, int width, int height) {
    this->x = x;
    this->y = y;
    buffer = new PixelBuffer(width, height);
}

Widget::~Widget() {
    delete buffer;
}

void Widget::draw() {
    buffer->clear(0xff0000);
    window->get_buffer()->blit(buffer, x, y, buffer->get_width(), buffer->get_height());
}

void Widget::set_window(Window* window) {
    this->window = window;
}

int Widget::get_width() {
    return buffer->get_width();
}

int Widget::get_height() {
    return buffer->get_height();
}

bool Widget::is_inside(int x, int y) {
    int x0 = this->x;
    int x1 = x0 + get_width();
    int y0 = this->y;
    int y1 = y0 + get_height();

    return x0 < x && x < x1 && y0 < y && y < y1;
}

void Widget::exec_callbacks(Event* event) {
    for (int i = 0; i < callbacks.size(); ++i) {
        callback_t cb = callbacks[i];
        cb(this, event);
    }
}

void Widget::add_callback(callback_t cb) {
    callbacks.push_back(cb);
}

void Widget::process_event(Event* event) {
    int x = event->get_x();
    int y = event->get_y();

    if (is_inside(x, y)) {
        exec_callbacks(event);
    }
}
