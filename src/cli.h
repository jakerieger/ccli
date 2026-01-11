#ifndef CLI_H
#define CLI_H

#include "common.h"

typedef struct cli_option cli_option;

typedef i32 (*cli_action)(cli_option* opts, u32 opt_count);

struct cli_option {
    const char* names;
    bool required;
    char value[256];
};

typedef struct {
    const char* names;
    cli_action action;
    const char* help_text;
    cli_option* options;
    u32 option_count;
} cli_command;

typedef struct {
    const char* name;
    const char* version;
    const char* description;

    cli_command* commands;
    u32 command_count;
} cli_app;

cli_app* cli_app_create(const char* name, const char* version, const char* description);
void cli_app_destroy(cli_app* app);
void cli_app_print_info(const cli_app* app);

cli_command* cli_app_add_command(cli_app* app, const char* names, cli_action action, const char* help_text);
void cli_cmd_add_option(cli_command* cmd, const char* names, bool required);

int cli_app_run(cli_app* app, const i32 argc, char** argv);

#endif
