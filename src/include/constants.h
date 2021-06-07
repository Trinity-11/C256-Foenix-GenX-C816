/**
 * Definitions of major constants
 */

//
// Target systems
//

#define TARGET_FMX      1       // Target the Foenix FMX
#define TARGET_U        2       // Target the Foenix U
#define TARGET_U_PLUS   3       // Target the Foenix U+
#define TARGET_GENX     4       // Target the Foenix GenX

//
// Special memory zones: heap, etc.
//

#if TARGET == TARGET_U
#define MAX_HEAP        0x16ffff
#define MIN_HEAP        0x010000
#elif TARGET == TARGET_U_PLUS || TARGET == TARGET_FMX
#define MAX_HEAP        0x36ffff
#define MIN_HEAP        0x010000
#endif

//
// Miscellaneous definitions
//

#define MAX_TRIES_BUSY  10000   // The maximum number of times to check for an operation to complete (general purpose)

//
// Definitions of special characters
//

#define CHAR_ESC '\x2b'         // Escape character
#define CHAR_NL '\r'            // Newline character
#define CHAR_BS '\b'            // Backspace
