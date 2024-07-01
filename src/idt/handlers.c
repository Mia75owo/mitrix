#include "handlers.h"

HandlerAdditionalInfo handler_additional_info;

#include "gfx/tty.h"
#include "gfx/vtty.h"
#include "util/debug.h"
#include "util/sys.h"

typedef struct {
    u32 present : 1;
    u32 write : 1;
    u32 user : 1;
    u32 reserved_write : 1;
    u32 instruction_fetch : 1;
    u32 protection_key : 1;
    u32 shadow_stack : 1;
    u32 software_guard_extension : 1;
} ErrorHandlerPageFault;

typedef struct {
    u32 external : 1;
    u32 table : 2;
    u32 index : 13;
} ErrorHandlerGeneralProtectionFault;

void handler_page_fault(CPUState* frame) {
    tty_clear();

    klog("%04ERROR: Exception (Page Fault)!\n");

    ErrorHandlerPageFault err;

    union { ErrorHandlerPageFault err; u32 val; } tmp = { .val = frame->error };
    err = tmp.err;

    if (!err.present) {
        klog("%40non-present page\n");
    }
    if (err.write) {
        klog("%04WRITE\n");
    } else {
        klog("%04READ\n");
    }
    if (err.user) {
        klog("%04USER\n");
    } else {
        klog("%04KERNEL\n");
    }
    if (err.reserved_write) {
        klog("%40INVALID PDE\n");
    }
    if (err.instruction_fetch) {
        klog("%04FETCH\n");
    }
    if (err.protection_key) {
        klog("%40PROTECTION KEY");
    }
    if (err.shadow_stack) {
        klog("%40SHADOW STACK ACCESS");
    }
    if (err.software_guard_extension) {
        klog("%40SOFTWARE GUARD EXTENSION");
    }

    klog("%0F%x\n", (u64)handler_additional_info.cr2);
    klog("%0A%x\n", (u64)handler_additional_info.eip);

    vtty_render();
    abort();
}

void handler_general_protection_fault(CPUState* frame) {
    tty_clear();

    klog("%04ERROR: Exception (General Protection Fault)!\n");

    ErrorHandlerGeneralProtectionFault err;
    union { ErrorHandlerGeneralProtectionFault err; u32 val; } tmp = { .val = frame->error };
    err = tmp.err;

    if (frame->error) {
        if (err.external) {
            klog("%04EXTERNAL\n");
        }

        if (err.table == 0) {
            klog("%04GDT\n");
        } else if (err.table == 1) {
            klog("%04IDT\n");
        } else if (err.table == 2) {
            klog("%04LDT\n");
        } else if (err.table == 3) {
            klog("%04IDT\n");
        }

        klog("INDEX: (%n)\n", (u64)err.index);
    } else {
        klog("%40Not segment related!\n");
    }

    klog("%0A%x\n", (u64)handler_additional_info.eip);

    vtty_render();
    abort();
}
