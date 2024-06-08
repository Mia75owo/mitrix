#ifndef EVENTS_H_
#define EVENTS_H_

#include "keyboard/keyboard.h"
#include "mouse/mouse.h"
#include "util/types.h"

#define MAX_EVENT_RECEIVERS 16
#define MAX_EVENTS 64

typedef union {
    u8 is_keyevent : 1;
    KeyEvent keyevt;
    MouseEvent mouseevt;
} Event;
_Static_assert(sizeof(Event) == 3, "Event union size mismatch");

typedef struct {
    u32* buf;
} EventReceiver;

typedef struct {
    u32 read_index;
    u32 write_index;
    Event events[MAX_EVENTS];
} EventBuffer;

void events_init();

void events_add_receiver(u32* buf);
void events_remove_receiver(u32* buf);

void events_key_event(KeyEvent evt);
void events_mouse_event(MouseEvent evt);

#endif
