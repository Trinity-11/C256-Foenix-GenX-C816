/**
 * Definitions for PS/2 interface mouse and keyboard.
 */

#ifndef __PS2_H
#define __PS2_H

#include "constants.h"
#include "types.h"

//
// PS/2 I/O ports
//

#if TARGET == TARGET_U || TARGET == TARGET_U_PLUS
#define PS2_STATUS          0xAF1807
#define PS2_CMD_BUF         0xAF1807
#define PS2_OUT_BUF         0xAF1803
#define PS2_INPT_BUF        0xAF1803
#define PS2_DATA_BUF        0xAF1803
#elif TARGET == TARGET_FMX
#define PS2_STATUS          0xAF1064
#define PS2_CMD_BUF         0xAF1064
#define PS2_OUT_BUF         0xAF1060
#define PS2_INPT_BUF        0xAF1060
#define PS2_DATA_BUF        0xAF1060
#endif

//
// ps2_init status code flags
//

#define PS2_FAIL_SELFTEST   -1
#define PS2_FAIL_KBDTEST    -2

#define PS2_KBD_MISSING     0x0001
#define PS2_MOUSE_MISSING   0x0002

//
// Initialize the PS2 controller and any attached devices
// Enable keyboard and mouse interrupts as appropriate.
//
// Returns:
//  Status code indicating if either the mouse or the keyboard is missing.
//
extern short ps2_init();

//
// Try to retrieve the next scancode from the keyboard.
//
// Returns:
//      The next scancode to be processed, 0 if nothing.
//
extern word kbd_get_scancode();

//
// Try to get a character from the keyboard...
//
// Returns:
//      the next character to be read from the keyboard (0 if none available)
//
extern char kbd_getc();

#endif