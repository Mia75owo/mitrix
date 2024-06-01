#include "events.h"

#include "util/debug.h"
#include "util/mem.h"

static EventReceiver receivers[MAX_EVENT_RECEIVERS];

void events_init() { memset(receivers, 0, sizeof(receivers)); }

static i32 get_free_slot() {
    for (u32 i = 0; i < MAX_EVENT_RECEIVERS; i++) {
        if (receivers[i].buf == 0) {
            return i;
        }
    }
    return -1;
}

void events_key_event(KeyEvent evt) {
    for (u32 i = 0; i < MAX_EVENT_RECEIVERS; i++) {
        if (receivers[i].buf == 0) continue;

        EventBuffer* buf = (EventBuffer*)receivers[i].buf;
        if (buf->count >= MAX_EVENTS) continue;

        buf->events[buf->count++] = evt;
    }
}

void events_add_receiver(u32* buf) {
    i32 slot = get_free_slot();
    assert(slot >= 0);

    receivers[slot].buf = buf;
}
void events_remove_receiver(u32* buf) {
    for (u32 i = 0; i < MAX_EVENT_RECEIVERS; i++) {
        if (receivers[i].buf == buf) {
            receivers[i].buf = 0;
        }
    }
}
