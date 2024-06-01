#ifndef EVENTS_H_
#define EVENTS_H_

#include "keyboard/keyboard.h"
#include "util/types.h"

#define MAX_EVENT_RECEIVERS 16
#define MAX_EVENTS 64

typedef struct {
    u32* buf;
} EventReceiver;

typedef struct {
    u32 count;
    KeyEvent events[MAX_EVENTS];
} EventBuffer;

void events_init();

void events_add_receiver(u32* buf);
void events_remove_receiver(u32* buf);

void events_key_event(KeyEvent evt);

#endif
