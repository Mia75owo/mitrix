#include "events.h"

#include "util/debug.h"
#include "util/mem.h"

static EventReceiver receivers[MAX_EVENT_RECEIVERS];
static Task* focused_task;

void events_init() {
    memset(receivers, 0, sizeof(receivers));
    focused_task = NULL;
}

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
        // Don't send event if another task is focused
        if (focused_task != receivers[i].task && !receivers[i].always_listening)
            continue;

        EventBuffer* buf = (EventBuffer*)receivers[i].buf;
        buf->write_index++;
        if (buf->write_index >= MAX_EVENTS) {
            buf->write_index = 0;
        }

        // Key Event flag
        evt.reserved = 1;
        buf->events[buf->write_index] = (Event)evt;

        receivers[i].task->pending_events = true;
    }
}
void events_mouse_event(MouseEvent evt) {
    for (u32 i = 0; i < MAX_EVENT_RECEIVERS; i++) {
        if (receivers[i].buf == 0) continue;

        EventBuffer* buf = (EventBuffer*)receivers[i].buf;
        buf->write_index++;
        if (buf->write_index >= MAX_EVENTS) {
            buf->write_index = 0;
        }

        // Key Event flag
        evt.reserved = 0;
        buf->events[buf->write_index] = (Event)evt;

        receivers[i].task->pending_events = true;
    }
}

void events_add_receiver(u32* buf, Task* task, bool always_listening) {
    i32 slot = get_free_slot();
    assert(slot >= 0);

    receivers[slot].buf = buf;
    receivers[slot].task = task;
    receivers[slot].always_listening = always_listening;
}
void events_remove_receiver(u32* buf) {
    for (u32 i = 0; i < MAX_EVENT_RECEIVERS; i++) {
        if (receivers[i].buf == buf) {
            receivers[i].buf = 0;
            // Unfocus task
            if (receivers[i].task == focused_task) {
                focused_task = NULL;
            }
        }
    }
}

void events_focus_task(Task* task) { focused_task = task; }
Task* events_get_focused_task() { return focused_task; }
