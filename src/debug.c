/**
 * Implementation of debug routines
 */

#include "debug.h"
#include "dev/channel/text_screen.h"

#ifdef DEBUG

void dbg_trace(char * message) {
    while (*message) {
        txt_raw_putc(0, *message++);
    }
    txt_raw_putc(0, '\r');
}

void dbg_log(char * message) {
    while (*message) {
        txt_raw_putc(0, *message++);
    }
}

#endif