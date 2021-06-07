/**
 * Code to manage the SN76489 PSG.
 */

#ifndef __PSG_H
#define __PSG_H

#include "types.h"

#define PSG_BASE_ADDRESS 0xAFF100

extern void psg_set_atten(byte voice, byte attenuation);
extern void psg_tone(byte voice, short frequency);
extern void psg_init();

#endif