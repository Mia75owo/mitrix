#include "shell.h"

#include "disk/mifs.h"
#include "gfx/gfx.h"
#include "gfx/tty.h"
#include "memory/kmalloc.h"
#include "memory/memory.h"
#include "pit/pit.h"
#include "tasks/task_manager.h"
#include "util/debug.h"
#include "util/mem.h"
#include "util/sys.h"

static ShellCommand commands[SHELL_COMMANDS_SIZE];

void shell_init() {
    memset(commands, 0, sizeof(commands));

    // clang-format off
    shell_add_command("help",
                      "Display information about available commands",
                      CMD_HELP);

    shell_add_command("clear",
                      "Clear the screen",
                      CMD_CLEAR);

    shell_add_command("reboot",
                      "Reboot the OS",
                      CMD_REBOOT);
    shell_add_command("shutdown",
                      "Shutdown the OS",
                      CMD_SHUTDOWN);
    shell_add_command("quit",
                      "Shutdown the OS",
                      CMD_SHUTDOWN);

    shell_add_command("meminfo",
                      "Display the memory map provided by GRUB",
                      CMD_MEMINFO);
    shell_add_command("mallocinfo",
                      "Display the content of the KMALLOC buddy bitmaps",
                      CMD_MALLOCINFO);
    shell_add_command("tics",
                      "Display the number of tics recorded by the PIT",
                      CMD_TICS);
    shell_add_command("debuggfx",
                      "DEBUG gfx text test",
                      CMD_DEBUGGFX);
    shell_add_command("debuggfx2",
                      "DEBUG gfx gradiend test",
                      CMD_DEBUGGFX2);
    shell_add_command("debugint",
                      "DEBUG trigger the DEBUG interrupt",
                      CMD_DEBUGINT);

    shell_add_command("logo",
                      "Display the MITRIX logo",
                      CMD_LOGO);
    shell_add_command("ls",
                      "List the files from disk",
                      CMD_LS);
    shell_add_command("cat",
                      "Print file contents to screen",
                      CMD_CAT);
    shell_add_command("exec",
                      "Jump the execution to a file (dangerous)",
                      CMD_EXEC);
    // clang-format on
}

void shell_add_command(const char* name, const char* description,
                       ShellCommandID id) {
    i32 index = -1;
    for (u32 i = 0; i < SHELL_COMMANDS_SIZE; i++) {
        if (commands[i].command_length == 0) {
            index = i;
            break;
        }
    }
    assert_msg(index != -1, "SHELL_COMMANDS_SIZE exeedet!");

    strncpy(commands[index].name, name, SHELL_COMMAND_SIZE);
    strncpy(commands[index].description, description, SHELL_DESCRIPTION_SIZE);
    commands[index].command_length = strlen(commands[index].name);
    commands[index].command_ID = id;
}

i32 shell_get_command_index(const char* command) {
    for (u32 i = 0; i < SHELL_COMMANDS_SIZE; i++) {
        if (strncmp(commands[i].name, command, commands[i].command_length) ==
                0 &&
            (command[commands[i].command_length] == '\0' ||
             command[commands[i].command_length] == ' ')) {
            return i;
        }
    }
    return -1;
}

bool shell_is_command(const char* command) {
    return shell_get_command_index(command) != -1;
}

void shell_execute_command(const char* command) {
    i32 index = shell_get_command_index(command);
    if (index == -1) return;

    u32 cmd = commands[index].command_ID;

    // TODO: cleanup this mess D:
    switch (cmd) {
        case CMD_HELP: {
            klog("\n%[50|=]");

            if (command[commands[index].command_length + 1] != '\0') {
                i32 idx = shell_get_command_index(
                    &command[commands[index].command_length + 1]);
                if (idx == -1) {
                    klog("\n%0CCommand '%s' not found!",
                         &command[commands[index].command_length + 1]);
                    break;
                }
                klog("%0F%s -> %s\n", commands[idx].name,
                     commands[idx].description);
            } else {
                klog("%0FUse 'help [COMMAND]' to get a description\n\n");
                for (u32 i = 0; i < SHELL_COMMANDS_SIZE; i++) {
                    if (commands[i].command_length == 0) break;
                    klog("%0F%s\n", commands[i].name);
                }
            }

            klog("%[50|=]");
        } break;
        case CMD_CLEAR: {
            tty_clear();
        } break;
        case CMD_REBOOT: {
            reboot();
        } break;
        case CMD_SHUTDOWN: {
            shutdown();
        } break;
        case CMD_MEMINFO: {
            memory_print_info();
        } break;
        case CMD_MALLOCINFO: {
            kmalloc_print_info();
        } break;
        case CMD_TICS: {
            klog("\n%n", (u64)pit_get_tics());
        } break;
        case CMD_DEBUGGFX: {
            gfx_debug(GFX_DEBUG_FONT_FILL);
            sleep(1000);
            gfx_fill(0xFF000000);
        } break;
        case CMD_DEBUGGFX2: {
            gfx_debug(GFX_DEBUG_GRADIENT);
            sleep(1000);
            gfx_fill(0xFF000000);
        } break;
        case CMD_DEBUGINT: {
            asm volatile("int $1");
        } break;
        case CMD_LOGO: {
            gfx_logo();
            sleep(1000);
        } break;
        case CMD_LS: {
            klog("\n%[50|=]");

            FilePtr file;
            u32 i = 0;
            while (true) {
                file = mifs_file_by_index(i);
                if (file.addr == 0) break;

#define SIZE_BUF_LEN 8

                static char size_buffer[SIZE_BUF_LEN];
                itoa(size_buffer, file.size, SIZE_BUF_LEN, 10);
                u32 size_len = strlen(size_buffer);

                klog("%0FSIZE:%0C%s", size_buffer);
                for (u32 i = 0; i < SIZE_BUF_LEN - size_len; i++) klog(" ");

                klog("%0F| NAME:%0A%s\n", file.name);

                i++;
            }

            klog("%[50|=]");
        } break;
        case CMD_CAT: {
            static char file_name_buffer[FNAME_SIZE];
            strncpy(file_name_buffer, command + 4, FNAME_SIZE);

            FilePtr file = mifs_file(file_name_buffer);

            if (file.addr == 0) {
                klog("\n%0CFile '%s' not found!", file_name_buffer);
            } else {
                klog("\n");

                for (u32 i = 0; i < file.size; i++) {
                    klog("%0F%c", file.addr[i]);
                }
            }
        } break;
        case CMD_EXEC: {
            static char file_name_buffer[FNAME_SIZE];
            strncpy(file_name_buffer, command + 5, FNAME_SIZE);

            FilePtr file = mifs_file(file_name_buffer);

            if (file.addr == 0) {
                klog("\n%0CFile '%s' not found!", file_name_buffer);
            } else {
                Task* new_task = create_user_task(file_name_buffer);
                (void)new_task;
            }
        } break;
    }
}
