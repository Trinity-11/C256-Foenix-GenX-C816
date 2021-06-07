//
// Definition of files for the SID implementation
//

#ifndef __SID_H
#define __SID_H

#include "types.h"

#define SID0_BASE 0xAFE400          // Location of the first byte of the SID chip

//
// Structure containing the SID registers
//
typedef struct s_sid {
    unsigned short v1_frequency;
    unsigned short v1_pulse_width;
    byte v1_control;
    byte v1_attack_decay;
    byte v1_sustain_release;

    unsigned short v2_frequency;
    unsigned short v2_pulse_width;
    byte v2_control;
    byte v2_attack_decay;
    byte v2_sustain_release;

    unsigned short v3_frequency;
    unsigned short v3_pulse_width;
    byte v3_control;
    byte v3_attack_decay;
    byte v3_sustain_release;   

    unsigned short filter_freq;
    byte res_filter;
    byte mode_volume;

    byte pot_x;     // Unused on the Foenix family
    byte pot_y;     // Unused on the Foenix family

    byte osc_rand;
    byte env3;

    byte unused1;
    byte unused2;
    byte unused3;
} t_sid;

extern void sid_init();

#endif