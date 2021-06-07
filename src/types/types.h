/**
 * Handy types for talking to the Foenix hardware
 */

#ifndef __TYPES_H
#define __TYPES_H

//
// Simple scalar types...
//

typedef unsigned char byte;         // 8-bits
typedef unsigned short word;        // 16-bits
typedef unsigned long dword;        // 32-bits

//
// A color (BGR)
//
typedef struct s_color3 {
    byte blue;
    byte green;
    byte red;
} t_color3;

//
// A color entry for a color lookup table (BGRA)
//
typedef struct s_color4 {
    byte blue;
    byte green;
    byte red;
    byte alpha;
} t_color4;

//
// Function types
//

typedef short (*FUNC_V_2_S)();
typedef short (*FUNC_S_2_S)(char *);
typedef short (*FUNC_BS_2_S)(byte *, short);
typedef short (*FUNC_cBS_2_S)(const byte *, short);
typedef short (*FUNC_B_2_S)(const byte);
typedef short (*FUNC_LBS_2_S)(long, byte *, short);
typedef short (*FUNC_LcBS_2_S)(long, const byte *, short);
typedef short (*FUNC_SBS_2_S)(short, byte *, short);
typedef short (*FUNC_LB_2_S)(long, short);

#endif
