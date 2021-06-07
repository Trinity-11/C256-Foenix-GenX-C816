//
// Definitions to support the clock and timers
//

#ifndef __CLOCK_H
#define __CLOCK_H

#include "types.h"

#define RTC_SEC       0xAF0800 //Seconds Register
#define RTC_SEC_ALARM 0xAF0801 //Seconds Alarm Register
#define RTC_MIN       0xAF0802 //Minutes Register
#define RTC_MIN_ALARM 0xAF0803 //Minutes Alarm Register
#define RTC_HRS       0xAF0804 //Hours Register
#define RTC_HRS_ALARM 0xAF0805 //Hours Alarm Register
#define RTC_DAY       0xAF0806 //Day Register
#define RTC_DAY_ALARM 0xAF0807 //Day Alarm Register
#define RTC_DOW       0xAF0808 //Day of Week Register
#define RTC_MONTH     0xAF0809 //Month Register
#define RTC_YEAR      0xAF080A //Year Register
#define RTC_RATES     0xAF080B //Rates Register
#define RTC_ENABLE    0xAF080C //Enables Register
#define RTC_FLAGS     0xAF080D //Flags Register
#define RTC_CTRL      0xAF080E //Control Register
#define RTC_CENTURY   0xAF080F //Century Register

typedef struct s_time {
    short year;
    short month;
    short day;
    short hour;
    short minute;
    short second;
} t_time, *p_time;

//
// Initialize the real time clock and timers
//
extern void clk_init();

//
// Get the current time
//
// Inputs:
//  time = the time structure  to fill with the current time
//
extern void clk_gettime(p_time time);

//
// Set the current time
//
// Inputs:
//  time = the time structure containing the current time  
//
extern void clk_settime(p_time time);

#endif
