#include "doomgeneric.h"
#include "doomkeys.h"
#include "events.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "syscalls.h"

#undef FEATURE_DEHACKED

#define HEAP_SIZE (0x1000 * 256 * 10)

u32* frame_buffer = NULL;
EventBuffer* event_buffer = NULL;

uint32_t DOOMGENERIC_RESX;
uint32_t DOOMGENERIC_RESY;

int _start() {
    syscall_set_heap_size(HEAP_SIZE);
    void* heap_start = syscall_get_heap_start();
    add_malloc_block(heap_start, HEAP_SIZE);

    printf("%ix%i\n", syscall_get_screen_size_x(), syscall_get_screen_size_y());
    DOOMGENERIC_RESX = syscall_get_screen_size_x();
    DOOMGENERIC_RESY = syscall_get_screen_size_y();

    char* argv[] = {"doom", "-iwad", "doom1.wad"};

    doomgeneric_Create(3, (char**)argv);

    while (true) {
        doomgeneric_Tick();
    }

    syscall_exit();
    return 0;
}

void DG_Init() {
    frame_buffer = syscall_create_fb(DOOMGENERIC_RESX, DOOMGENERIC_RESY);
    event_buffer = syscall_create_events_buf();

    events_init(event_buffer);
}

void DG_DrawFrame() {
    DG_ScreenBuffer = frame_buffer;
    syscall_draw_fb();
}
void DG_SleepMs(uint32_t ms) {
    u32 start = syscall_get_systime();
    while (syscall_get_systime() - start < ms) {
    }
}
uint32_t DG_GetTicksMs() { return syscall_get_systime(); }

static u8 to_doom_key(KeyEvent evt) {
    if (evt.special) {
        switch (evt.c) {
            case KEYCODE_UP:
                return KEY_UPARROW;
            case KEYCODE_LEFT:
                return KEY_LEFTARROW;
            case KEYCODE_DOWN:
                return KEY_DOWNARROW;
            case KEYCODE_RIGHT:
                return KEY_RIGHTARROW;
            case KEYCODE_ESCAPE:
                return KEY_ESCAPE;
        }
    } else {
        switch (evt.c) {
            case 'w':
                return KEY_UPARROW;
            case 'a':
                return KEY_STRAFE_L;
            case 's':
                return KEY_DOWNARROW;
            case 'd':
                return KEY_STRAFE_R;

            case 'j':
                return KEY_LEFTARROW;
            case 'k':
                return KEY_USE;
            case 'l':
                return KEY_RIGHTARROW;
            case ' ':
                return KEY_FIRE;
            case 'u':
                return KEY_USE;
            case '\n':
                return KEY_ENTER;
        }
    }

    return evt.c;
}

int DG_GetEvent(DG_Event* doom_evt) {
    if (!events_has_event()) return 0;

    Event rawevt = events_pull();

    if (rawevt.is_keyevent) {
        KeyEvent evt = rawevt.keyevt;
        doom_evt->is_key_event = 1;
        doom_evt->key_event.pressed = evt.pressed;
        doom_evt->key_event.key = to_doom_key(evt);
    } else {
        MouseEvent evt = rawevt.mouseevt;
        doom_evt->is_key_event = 0;
        doom_evt->mouse_event.left_mouse_button = evt.button_left;
        doom_evt->mouse_event.delta_x = evt.delta_x * 3;
        doom_evt->mouse_event.delta_y = evt.delta_y * 3;
    }

    return 1;
}

void DG_SetWindowTitle(const char* title) { (void)title; }
