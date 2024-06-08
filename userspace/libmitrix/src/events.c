#include "events.h"

static EventBuffer* buf;

void events_init(EventBuffer* buffer) {
    buf = buffer;
    buf->read_index = MAX_EVENTS - 1;
    buf->write_index = MAX_EVENTS - 1;
}
bool events_has_event() {
    return buf->read_index != buf->write_index;
}
Event events_pull() {
    if (!events_has_event()) {
        return (Event){0};
    }

    buf->read_index++;
    if (buf->read_index >= MAX_EVENTS - 1) {
        buf->read_index = 0;
    }
    return buf->events[buf->read_index];
}
