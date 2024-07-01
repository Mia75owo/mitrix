#include "mouse.h"

#include "events/events.h"
#include "gfx/gui.h"
#include "idt/idt.h"
#include "util/debug.h"
#include "util/mem.h"
#include "util/port.h"

#define PACKET_COUNT 3
static u8 packets[PACKET_COUNT];
static u8 packet_index;

static void mouse_write(u8 data);
static void port_wait_output() {
    for (u32 i = 0; i < 1000; i++) {
        if ((inb(0x64) & 2) == 0) return;
    }
}

void mouse_init() {
    memset(packets, 0, sizeof(packets));
    packet_index = 0;

    set_isr_function(44, mouse_handler);

    port_wait_output();
    outb(0x64, 0x20);
    u8 status = inb(0x60);
    status |= 2;
    outb(0x64, 0x60);
    outb(0x60, status);

    outb(0x64, 0xa8);

    mouse_write(0xf4);
}

static void mouse_write(u8 data) {
    port_wait_output();
    outb(0x64, 0xd4);
    port_wait_output();
    outb(0x60, data);
}

void mouse_handler(CPUState* frame) {
    (void)frame;
    u8 data = inb(0x60);
    packets[packet_index++] = data;

    if (packet_index < PACKET_COUNT) {
        return;
    }

    // Full packet
    packet_index = 0;

    MouseEvent evt;
    evt.delta_x = packets[2];
    evt.delta_y = -packets[0];
    evt.button_left = (packets[1] & (1 << 0)) > 0;
    evt.button_right = (packets[1] & (1 << 1)) > 0;

    Task* focused_task = events_get_focused_task();
    if (focused_task == NULL || focused_task->is_kernel_task) {
        gui_mouse_event(evt);
    }
    events_mouse_event(evt);
}
