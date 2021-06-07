/**
 * Define simple functions for debugging
 */

#ifndef __DEBUG_H
#define __DEBUG_H

// #define DEBUG

//
// Macros
//

#ifdef DEBUG

#define LOG(x) dbg_log(x);
#define TRACE(x) dbg_trace(x);

#else

#define LOG(x) ;
#define TRACE(x) ;


#endif

//
// Underlying routines
//

#ifdef DEBUG

extern void dbg_log(char * message);
extern void dbg_trace(char * message);

#endif

#endif