#ifndef EVENTS_H_
#define EVENTS_H_

#include "types.h"

// TODO: move this to a syncronized location (kernel <> userspace)
// clang-format off
#define KEYCODE_UP     100
#define KEYCODE_LEFT   101
#define KEYCODE_DOWN   102
#define KEYCODE_RIGHT  103
#define KEYCODE_MOD    104
#define KEYCODE_CTRL   105
#define KEYCODE_ESCAPE 106

#define KEYCODE_F1     110
#define KEYCODE_F2     111
#define KEYCODE_F3     112
#define KEYCODE_F4     113
#define KEYCODE_F5     114
#define KEYCODE_F6     115
#define KEYCODE_F7     116
#define KEYCODE_F8     117
#define KEYCODE_F9     118
#define KEYCODE_F10    119
#define KEYCODE_F11    120
#define KEYCODE_F12    121
// clang-format on

#define MAX_EVENTS 64

typedef struct {
    u8 pressed : 1;
    u8 shift : 1;
    u8 ctrl : 1;
    u8 alt : 1;
    u8 extended : 1;
    u8 special : 1;

    u8 raw;
    char c;
} KeyEvent;

typedef struct {
    u32 read_index;
    u32 write_index;
    KeyEvent events[MAX_EVENTS];
} EventBuffer;

void events_init(EventBuffer* buffer);
bool events_has_event();
KeyEvent events_pull();

#endif
