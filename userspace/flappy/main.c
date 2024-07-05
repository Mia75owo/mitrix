#include "events.h"
#include "gfx.h"
#include "stdio.h"
#include "stdlib.h"
#include "syscalls.h"

static float player_pos = 0;
static float player_vel = 0;
static u32 last_jump = 0;
static u32 last_draw = 0;

static float wanted_dt = 1.f / 30.f * 1000.f;
static float dt = 0.f;

static bool started = false;

typedef struct {
    float x;
    float y;
} Pipe;

static inline int rand_range(i32 low, i32 high) {
    return rand() % (high + 1 - low) + low;
}

#define PIPES_COUNT 3
static Pipe pipes[PIPES_COUNT];

int _start() {
    u32* frame_buffer = syscall_create_fb(800, 600, false);
    GfxInfo gfx_info;
    gfx_info.width = 800;
    gfx_info.height = 600;
    gfx_info.bpp = 8;
    gfx_info.pitch = 0;
    gfx_info.addr = frame_buffer;
    gfx_init(gfx_info);

    EventBuffer* event_buffer = syscall_create_events_buf();
    events_init(event_buffer);

    for (u32 i = 0; i < PIPES_COUNT; i++) {
        pipes[i].x = 800 + i * 300;
        pipes[i].y = 600.f / 2.f + rand_range(-170, 170);
    }

    while (true) {
        u32 systime = syscall_get_systime();
        u32 dt_ms = systime - last_draw;
        dt = (float)dt_ms / (float)wanted_dt;

        last_draw = systime;

        Event rawevt = events_pull();
        if (rawevt.is_keyevent) {
            KeyEvent evt = rawevt.keyevt;

            if (evt.c == 'q') {
                goto exit;
            } else if (evt.c == ' ') {
                started = true;
            }
        }

        if (!started) {
            goto render;
        }

        // Update game data
        player_vel += 3.0f * dt;
        player_pos += player_vel * dt;

        for (u32 i = 0; i < PIPES_COUNT; i++) {
            pipes[i].x -= 5.f * dt;

            if (pipes[i].x <= -40.f) {
                pipes[i].x = 800.f;
            }

            // Collision?
            if (pipes[i].x + 40 > 50 && pipes[i].x < 50 + 40) {
                if (pipes[i].y - 100 > player_pos ||
                    pipes[i].y + 100 < player_pos + 40) {
                    goto exit;
                }
            }
        }

        if (rawevt.is_keyevent) {
            KeyEvent evt = rawevt.keyevt;
            if (evt.c == ' ' && evt.pressed) {
                if (systime > last_jump + 100) {
                    player_vel = -20.5f;
                    last_jump = systime;
                }
            }
        }

        // Game lost
        if (player_pos > 600 - 40) {
            goto exit;
        }
        if (player_pos < 0) {
            player_pos = 0;
        }

    render:;
        gfx_fill(0xFF3380FF);
        gfx_rect(50, player_pos, 40, 40, 0xFFFF0000);

        for (u32 i = 0; i < PIPES_COUNT; i++) {
            i32 x = pipes[i].x;
            i32 width = 40;
            if (x < -40 || x > 800) {
                continue;
            }

            if (x < 0) {
                width += x;
                x = 0;
            }
            if (x > (800 - 40)) {
                width = 800 - x;
            }

            gfx_rect(x, 0, width, pipes[i].y - 100, 0xFF00FF00);
            gfx_rect(x, pipes[i].y + 100, width, 600 - (pipes[i].y + 100),
                     0xFF00FF00);
        }

        syscall_draw_fb(800, 600);
    }

exit:;

    gfx_fill(0xFFFF0000);
    syscall_draw_fb(800, 600);

    u32 time = syscall_get_systime();
    while (syscall_get_systime() < time + 1000)
        ;

    syscall_exit();
    return 0;
}
