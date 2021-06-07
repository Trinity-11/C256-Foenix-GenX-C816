//
// Code needed to support the CODEC
//

#include "dev/codec.h"

//
// Store data in the indicated codec register
//
void codec_set_register(byte reg, unsigned short data) {
    volatile unsigned short *data_port = (unsigned short *)CODEC_DATA;
    volatile byte *status = (byte *)CODEC_WR_CTRL;
    unsigned short cmd = ((reg & 0x7f) << 9) | (data & 0x1ff);

    *data_port = cmd;
    *status = 0x01;
    while ((*status & 0x01) != 0) {
        ;
    }
}

//
// Set up the CODEC with the initial settings
//
void codec_init() {
    codec_set_register(13, 0x000);  // R13 - enable DAC, ADC, headphone, analog inputs
    codec_set_register(21, 0x01E);  // R21 - Enable all analog inputs
    codec_set_register(17, 0x101);  // R17 - Enable gain control, ALC hold time 2.67ms
    codec_set_register(22, 0x007);  // R22 - Enable all analog outputs
    codec_set_register(10, 0x002);  // R10 - DAC I2S mode, 16-bit word
    codec_set_register(11, 0x002);  // R11 - ADC I2S mode, 16-bit word
    codec_set_register(12, 0x045);  // R12 - Master mode...
    codec_set_register(23, 0x000);  // R23 - Reset...
}
