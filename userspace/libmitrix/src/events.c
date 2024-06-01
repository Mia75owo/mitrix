#include "events.h"

static EventBuffer* buf;

void events_init(EventBuffer* buffer) {
    buf = buffer;
    buf->count = 0;
}
bool events_has_event() {
    return buf->count > 0;
}
KeyEvent events_pull() {
    if (buf->count == 0) {
        return (KeyEvent){0};
    }

    buf->count--;
    return buf->events[buf->count];
}
