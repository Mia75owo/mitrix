#ifndef SHELL_H_
#define SHELL_H_

#include "util/types.h"

#define SHELL_COMMAND_SIZE 32
#define SHELL_DESCRIPTION_SIZE 256
#define SHELL_COMMANDS_SIZE 32

typedef struct {
    char name[SHELL_COMMAND_SIZE];
    char description[SHELL_DESCRIPTION_SIZE];

    u32 command_length;
    u32 command_ID;
} ShellCommand;

typedef enum {
    CMD_HELP,
    CMD_CLEAR,

    CMD_REBOOT,
    CMD_SHUTDOWN,

    CMD_MEMINFO,
    CMD_MALLOCINFO,
    CMD_TICS,

    CMD_DEBUGGFX,
    CMD_DEBUGGFX2,
    CMD_DEBUGINT,

    CMD_LOGO,
    CMD_LS,
    CMD_CAT,
    CMD_EXEC,
} ShellCommandID;

void shell_init();

void shell_add_command(const char* name, const char* description,
                       ShellCommandID id);
i32 shell_get_command_index(const char* command);
bool shell_is_command(const char* command);
void shell_execute_command(const char* command);

#endif
