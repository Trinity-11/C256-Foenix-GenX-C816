//
// Definitions for the WM8776 CODEC
//

#ifndef __CODEC_H
#define __CODEC_H

#include "types.h"

#define CODEC_DATA      0xAFE900                                // (W) 16 bit Add/Data Reg to Control CODEC See WM8776 Spec
#define CODEC_WR_CTRL   0xAFE902                                // (W) Bit[0] = 1 -> Start Writing the CODEC Control Register

extern void codec_set_register(byte reg, unsigned short data);  // Send a command or data word (16-bits) to the CODEC
extern void code_init();                                        // Do the initial setup of the CODEC

#endif
