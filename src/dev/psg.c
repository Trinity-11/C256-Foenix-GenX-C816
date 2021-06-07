/**
 * Control code needed for the SN76489 sound chip
 */

#include "dev/psg.h"

void psg_set_atten(byte voice, byte attenuation) {
    volatile byte *psg_port = (byte *)PSG_BASE_ADDRESS;
    byte command = 0x90 | (voice & 0x03 << 5) | (attenuation & 0x0f);   // Generate the attenuation command
    *psg_port = command;
}

/**
 * Play a tone on the designated voice of the PSG
 */
void psg_tone(byte voice, short frequency) {
    volatile byte *psg_port = (byte *)PSG_BASE_ADDRESS;
    byte command1 = 0x80 | (voice & 0x03 << 5) | (frequency & 0x0f);    // Generate the first frequency command
    byte command2 = ((frequency >> 4) & 0x3f);                          // Generate the second frequency command
    *psg_port = command1;
    *psg_port = command2;
}

/**
 * Initialize the SN76477 by turning all the voices to maximum attenuation.
 */
void psg_init() {
    short voice;
    for (voice = 0; voice < 4; voice++) {
        psg_set_atten(voice, 0x0f);
    }
}
