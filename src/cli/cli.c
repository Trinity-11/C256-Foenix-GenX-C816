/**
 * Implementation of the command line interface
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "cli/cli.h"
#include "cli/dos_commands.h"
#include "dev/channel/channel.h"

//
// Types
//

// Structure to hold a record about a command...

typedef struct s_cli_command {
    char *name;
    FUNC_S_2_S handler;
    struct s_cli_command *next;
} t_cli_command, *p_cli_command;

//
// CLI variables
//

p_cli_command g_cli_commands;

//
// Register a command with the CLI
//
// Inputs:
//  name = the name of the command (comparison is case insensitive)
//  handler = pointer to the string->short function that implements the command
//
short cli_register(char * name, FUNC_S_2_S handler) {
    // Allocate a new command record
    p_cli_command command = malloc(sizeof(t_cli_command));

    if (command > 0) {
        int i;
        int size;

        // If that was successful... allocate space for a copy of the name...
        size = strlen(name) + 1;
        command->name = malloc(size);
        if (command->name != 0) {
            char * x = command->name;
            char * y = name;

            do {
                if (islower(*y)) {
                    *x++ = toupper(*y++);
                } else {
                    *x++ = *y++;
                }
            } while(*y);
            *x = 0;

            // Link it into the commands linked list
            command->handler = handler;
            command->next = g_cli_commands;
            g_cli_commands = command;
        } else {
            // If we couldn't allocate a copy of the name, free up the command
            free(command);

            // And return an error
            return -1;
        }
    } else {
        // Return an error
        return -1;
    }
}

const int cli_command_size = 128;

const char *cli_cmd_hello_message = "Executing command HELLO:\rHello, world!... Parameters: [";

//
// A silly test command
//
short cli_cmd_hello(char * parameters) {
    chan_write(0, cli_cmd_hello_message, strlen(cli_cmd_hello_message));
    chan_write(0, parameters, strlen(parameters));
    chan_write(0, "]\r", 2);
    return 0;
}

//
// Attempt to execute a command
//
// Inputs:
//  command = the upper case name of the command (first word of the command line)
//  parameters = the string of parameters to be passed to the command
//
short cli_exec(char * command, char * parameters) {
    p_cli_command commands;

    commands = g_cli_commands;

    while (commands != 0) {
        // Does the command match the name?
        if (strcmp(commands->name, command) == 0) {
            // Found it, execute the handler
            return commands->handler(parameters);
        } else {
            // No match, keep checking...
            commands = commands->next;
        }
    }

    // Built in command not found..
    // TODO: search the current drive for an executable file
    chan_write(0, "\rCommand not found.", 6);
    return -1;
}

//
// Enter the CLI's read-eval-print loop
//
short cli_repl() {
    char * command_line;                // Buffer for a full line
    char * command;                     // Pointer to the start of the command
    char * parameters;                  // Pointer to the start of the parameters

    // Allocate our command line buffer
    command_line = malloc(cli_command_size);
    if (command_line == 0) {
        return -1;
    }

    // Allocate our command buffer
    command = malloc(cli_command_size);
    if (command_line == 0) {
        return -1;
    }

    while (1) {
        chan_write(0, "\r] ", 2);                           // Print our prompt
        chan_readline(0, command_line, cli_command_size);   // Attempt to read line

        // Start at the beginning of the command line
        command = command_line;

        // Skip over leading white space
        while (*command != 0 && isspace(*command)) {
            command++;
        }

        // Start at the beginning of the command
        parameters = command;

        // Skip over any non-space character... converting to uppercase as we go
        while (*parameters != 0 && !isspace(*parameters)) {
            if (islower(*parameters)) {
                *parameters = toupper(*parameters);
            }
            parameters++;
        }

        // If we're not at the end of the line
        if (*parameters != 0) {
            // Mark this spot with a 0... end of command
            *parameters++ = 0;
        }

        // If we are at a space, move to the first non-space character
        while (*parameters != 0 && isspace(*parameters)) {
            parameters++;
        }

        // Try to execute the command
        cli_exec(command, parameters);
    }

    // Return our buffers
    free(command);
    free(command_line);
}

//
// Initialize the CLI
//
// Returns:
//  0 on success, negative number on error
//
short cli_init() {
    g_cli_commands = 0;

    cli_register("HELLO", cli_cmd_hello);
    cli_dos_register();

    return 0;
}