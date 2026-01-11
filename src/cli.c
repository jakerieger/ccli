#include "cli.h"

static char** split_names(const char* names, size_t* name_count) {
    char** buffer = NULL;
    *name_count   = 0;

    const char* delim = ",";
    char* str_cpy     = strdup(names);
    char* token       = strtok(str_cpy, delim);

    while (token != NULL) {
        buffer              = realloc(buffer, sizeof(char*) * (*name_count + 1));
        buffer[*name_count] = strdup(token);
        (*name_count)++;
        token = strtok(NULL, delim);
    }

    free(str_cpy);
    return buffer;
}

cli_app* cli_app_create(const char* name, const char* version, const char* description) {
    cli_app* app = (cli_app*)malloc(sizeof(cli_app));

    app->name        = name;
    app->version     = version;
    app->description = description;

    app->commands      = NULL;
    app->command_count = 0;

    return app;
}

void cli_app_destroy(cli_app* app) {
    for (int i = 0; i < app->command_count; i++) {
        cli_command cmd = app->commands[i];
        free(cmd.options);
    }

    free(app->commands);
    free(app);
}

void cli_app_print_info(const cli_app* app) {
    printf("Name: %s\nVersion: %s\nDescription: %s\n", app->name, app->version, app->description);
}

cli_command* cli_app_add_command(cli_app* app, const char* names, cli_action action, const char* help_text) {
    app->commands = (cli_command*)realloc(app->commands, sizeof(cli_command) * (app->command_count + 1));
    if (app->commands == NULL) {
        perror("alloc");
        exit(1);
    }
    cli_command cmd = {
      .names        = names,
      .action       = action,
      .help_text    = help_text,
      .options      = NULL,
      .option_count = 0,
    };
    app->commands[app->command_count] = cmd;
    return &app->commands[app->command_count++];
}

void cli_cmd_add_option(cli_command* cmd, const char* names, bool required) {
    cmd->options = (cli_option*)realloc(cmd->options, sizeof(cli_option) * (cmd->option_count + 1));
    if (cmd->options == NULL) {
        perror("alloc");
        exit(1);
    }

    cli_option opt = {
      .names    = names,
      .required = required,
      .value    = {'\0'},
    };

    cmd->options[cmd->option_count++] = opt;
}

static size_t calc_longest_cmd(const cli_command* cmds, const size_t count) {
    size_t longest = 0;
    for (int i = 0; i < count; i++) {
        const cli_command cmd = cmds[i];
        const char* name      = cmd.names;
        const size_t name_len = strlen(name);
        if (name_len > longest) {
            longest = name_len;
        }
    }
    return longest;
}

static void print_help_text(const cli_app* app) {
    printf("%s - %s\n%s\n\n", app->name, app->version, app->description);
    if (app->command_count > 0) {
        printf("COMMANDS\n");

        size_t longest = calc_longest_cmd(app->commands, app->command_count);
        for (int i = 0; i < app->command_count; i++) {
            const cli_command cmd    = app->commands[i];
            size_t currently_printed = 0;

            printf("  ");

            const size_t name_len = strlen(cmd.names);
            printf("%s", cmd.names);
            currently_printed += name_len;

            if (currently_printed < longest) {
                const size_t diff = longest - currently_printed;
                for (size_t i = 0; i < diff; i++) {
                    printf(" ");
                }
            }

            printf("    ");
            printf("%s\n", cmd.help_text);
        }

        printf("\n\n");
    }
}

int cli_app_run(cli_app* app, const i32 argc, char** argv) {
    if (app->command_count > 0 && argc < 2) {
        print_help_text(app);
        return 1;
    }

    if (argc > 2) {
        const char* command = argv[1];
        for (int i = 0; i < app->command_count; i++) {
            const cli_command cmd = app->commands[i];
            size_t name_count;
            char** names = split_names(cmd.names, &name_count);

            for (size_t k = 0; k < name_count; k++) {
                const char* name = names[k];
                if (strcmp(name, command) == 0) {
                    i32 remaining         = argc - 2;  // Get remaining argument count
                    bool has_required_arg = false;
                    if (cmd.option_count != 0) {
                        for (int l = 0; l < cmd.option_count; l++) {
                            if (cmd.options[l].required)
                                has_required_arg = true;
                        }
                    }

                    if (remaining == 0 && has_required_arg) {
                        fprintf(stderr, "error: command '%s' has required option(s).\n", name);
                        return 1;
                    } else if (remaining > 0) {
                        // iterate through remaining arguments
                        int arg_idx                  = 2;
                        char* current                = argv[arg_idx];
                        cli_option provided_opts[64] = {{}};
                        size_t provided_opts_count   = 0;
                        for (int l = 0; l < cmd.option_count; l++) {
                            cli_option opt = cmd.options[l];
                            size_t opt_name_count;
                            char** opt_names = split_names(opt.names, &opt_name_count);
                            for (int x = 0; x < opt_name_count; x++) {
                                if (strcmp(opt_names[x], current) == 0) {
                                    // get value
                                    if (argc > arg_idx) {
                                        char* value    = argv[++arg_idx];
                                        size_t val_len = strlen(value);
                                        if (val_len < 256) {
                                            memcpy(opt.value, value, val_len);
                                        }

                                        provided_opts[provided_opts_count++] = opt;
                                    }
                                }
                            }
                            current = argv[++arg_idx];
                        }

                        return cmd.action(provided_opts, provided_opts_count);
                    }
                }
            }
        }

        fprintf(stderr, "error: at least one command is required.\n");
        print_help_text(app);
        return 1;
    }

    return 0;
}
