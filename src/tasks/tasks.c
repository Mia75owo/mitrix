#include "tasks.h"

#include "disk/mifs.h"
#include "elf/elf.h"
#include "memory/kmalloc.h"
#include "memory/memory.h"
#include "memory/pmm.h"
#include "tasks/tss.h"
#include "util/debug.h"
#include "util/mem.h"
#include "util/sys.h"

extern void isr_exit();
extern void switch_context(Task* old, Task* next);

static void task_create(Task* this, void entrypoint(), bool kernel_task,
                        u32* pagedir) {
    this->stack = kmalloc(TASK_STACK_SIZE);

    u8* kesp0 = this->stack + TASK_STACK_SIZE;
    u8* kesp = kesp0;

    kesp -= sizeof(CPUState);
    this->cpustate = (CPUState*)(kesp);

    memset(this->cpustate, 0, sizeof(CPUState));

    // Setup selectors

    // NOTE: these values must equal to the offset of the
    // selectors in the GDT
    u32 code_selector;
    u32 data_selector;

    if (kernel_task) {
        code_selector = 8;
        data_selector = 16;
    } else {
        code_selector = 24 | 3;
        data_selector = 32 | 3;
    }

    this->cpustate->cs = code_selector;
    this->cpustate->ds = data_selector;
    this->cpustate->es = data_selector;
    this->cpustate->fs = data_selector;
    this->cpustate->gs = data_selector;

    // Return context
    kesp -= sizeof(TaskReturnContext);
    TaskReturnContext* context = (TaskReturnContext*)kesp;
    context->edi = 0;
    context->esi = 0;
    context->ebx = 0;
    context->ebp = 0;
    context->eip = (u32)isr_exit;

    this->cpustate->ss = data_selector;
    this->cpustate->esp = USER_STACK_BOTTOM;

    this->cpustate->eip = (u32)entrypoint;
    this->cpustate->eflags = 0x200;

    this->page_dir = pagedir;
    this->kesp0 = (u32)kesp0;
    this->kesp = (u32)kesp;

    if (kernel_task) {
        this->shmem_pool.vaddr_start = KERNEL_SHARED_MEM;
    } else {
        this->shmem_pool.vaddr_start = USER_SHARED_MEM;
    }
    this->shmem_fb_obj = -1;
    this->shmem_events_obj = -1;
}

void task_kernel_create(Task* this, void entrypoint()) {
    this->raw_elf = 0;
    task_create(this, entrypoint, true, initial_page_dir);
}

void task_user_create(Task* this, char* elf_file_name) {
    cli_push();
    ELFObject elf = {0};

    FilePtr file = mifs_file(elf_file_name);

    assert_msg(file.addr, "Failed to load file for user task!");
    elf.size = file.size;

    assert_msg(elf.size >= 52, "ELF file to small");
    elf.raw = kmalloc(elf.size);
    memcpy(elf.raw, file.addr, file.size);
    this->raw_elf = elf.raw;

    u32* page_dir = memory_get_current_page_dir();
    u32* new_page_dir = memory_alloc_page_dir();
    memory_change_page_dir(new_page_dir);

    if (!elf_parse(&elf)) {
        assert_msg(0, "Failed to parse ELF");
    }
    if (!elf_load_executable(&elf)) {
        assert_msg(0, "Failed to load ELF");
    }
    assert_msg(elf.entry, "ELF has no entry");

    // allocate user stack
    for (u32 i = 0; i < USER_STACK_PAGES; i++) {
        u32 virt_addr =
            USER_STACK_BOTTOM - USER_STACK_PAGES * 0x1000 + i * 0x1000;
        u32 phys_addr = pmm_alloc_pageframe();
        u32 flags = PTE_OWNER | PTE_USER | PTE_READ_WRITE;

        memory_map_page(virt_addr, phys_addr, flags);
    }

    task_create(this, (void(*))elf.entry, false, new_page_dir);

    assert(page_dir != NULL);
    assert(new_page_dir != NULL);

    memory_change_page_dir(page_dir);

    cli_pop();
}

void task_kill(Task* task) {
    cli_push();

    memory_free_page_dir(task->page_dir);
    kfree(task->stack);
    if (task->raw_elf) {
        kfree(task->raw_elf);
    }
    memset(task, 0, sizeof(Task));

    cli_pop();
}

void task_switch(Task* old, Task* new) {
    tss_update_esp0((u32) new->kesp0);
    switch_context(old, new);
}
