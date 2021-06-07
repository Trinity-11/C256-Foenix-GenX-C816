/**
 * Command Line Interface
 */

#ifndef __CLI_H
#define __CLI_H

#include "types.h"

//
// Initialize the CLI
//
// Returns:
//  0 on success, negative number on error
//
extern short cli_init();

//
// Register a command with the CLI
//
// Inputs:
//  name = the name of the command (comparison is case insensitive)
//  handler = pointer to the string->short function that implements the command
//
extern short cli_register(char * name, FUNC_S_2_S handler);

//
// Enter the CLI's read-eval-print loop
//
extern short cli_repl();

#endif