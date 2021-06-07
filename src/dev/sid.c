/**
 * Code to support the SID chip
 */

#include "dev/sid.h"

/**
 * Initialize the SID chip by setting everything to 0.
 */
void sid_init() {
    volatile byte *sid = (byte *)SID0_BASE;
    int x;

    // zero out the first 24 bytes of the SID chip
    for (x = 0; x < 24; x++) {
        sid[x] = 0;
    }
}