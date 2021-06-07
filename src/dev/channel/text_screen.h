/**
 * Definitions for low-level support of the text screens
 */

#ifndef __TEXT_SCREEN_H
#define __TEXT_SCREEN_H

#include "../types/types.h"

extern void txt_scr_init();                     // Initialize the text screens
extern void txt_putc(int screen, char c);       // Print a character to the indicated screen
extern void txt_raw_putc(int screen, char c);   // Print a character to the indicated screen
extern void txt_prhex_b(int screen, byte data); // Print a byte in hex
extern void txt_prhex_w(int screen, short data);// Print a word in hex
extern void txt_setsize(int screen);            // Recompute the size of the screen based on the hardware register settings
extern void txt_scr_clear(int screen);          // Clear the screen
extern void txt_scr_scrollup(int screen);       // Scroll the screen up one line
extern void txt_locate(int screen, short x, short y);

#endif
