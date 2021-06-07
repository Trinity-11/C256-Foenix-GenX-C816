/**
 * Definitions supporting the clock and timers
 */

#include "vicky_general.h"
#include "types.h"
#include "dev/clock.h"
#include "../interrupt.h"

//
// Initialize the real time clock and timers
//
void clk_init() {
    volatile byte *rates = (byte *)RTC_RATES;
    volatile byte *enable = (byte *)RTC_ENABLE;
    volatile byte *control = (byte *)RTC_CTRL;

    *rates = 0;         // Set watch dog timer and periodic interrupt rates to 0
    *enable = 0;        // Disable all the alarms and interrupts
    *control = 0x04;    // Make sure the RTC will continue to tick in battery mode
}

//
// Get the current time
//
// Inputs:
//  time = the time structure  to fill with the current time
//
void clk_gettime(p_time time) {
    volatile byte * r_control = (byte *)RTC_CTRL;
    volatile byte * r_century = (byte *)RTC_CENTURY;
    volatile byte * r_year = (byte *)RTC_YEAR;
    volatile byte * r_month = (byte *)RTC_MONTH;
    volatile byte * r_day = (byte *)RTC_DAY;
    volatile byte * r_hour = (byte *)RTC_HRS;
    volatile byte * r_minute = (byte *)RTC_MIN;
    volatile byte * r_second = (byte *)RTC_SEC;

    byte raw_century;
    byte raw_year;
    byte raw_month;
    byte raw_day;
    byte raw_hour;
    byte raw_minute;
    byte raw_second;

    //
    // Fetch the raw data from the RTC
    //

    *r_control = *r_control | 0x08;             // Set UTI bit to prevent updates;
    raw_century = *r_century;
    raw_year = *r_year;
    raw_month = *r_month;
    raw_day = *r_day;
    raw_hour = *r_hour;
    raw_minute = *r_minute;
    raw_second = *r_second;
    *r_control = *r_control & 0xF7;             // Clear UTI bit to prevent updates;

    //
    // Convert from BCD to binary values
    //

    time->year = ((raw_century >> 4) & 0x0f) * 1000 + (raw_century & 0x0f) * 100 + ((raw_year >> 4) & 0x0f) * 10 + (raw_year & 0x0f);
    time->month = ((raw_month >> 4) & 0x0f) * 10 + (raw_month & 0x0f);
    time->day = ((raw_day >> 4) & 0x0f) * 10 + (raw_day & 0x0f);
    time->hour = ((raw_hour >> 4) & 0x0f) * 10 + (raw_hour & 0x0f);
    time->minute = ((raw_minute >> 4) & 0x0f) * 10 + (raw_minute & 0x0f);
    time->second = ((raw_second >> 4) & 0x0f) * 10 + (raw_second & 0x0f);
}

//
// Set the current time
//
// Inputs:
//  time = the time structure containing the current time  
//
void clk_settime(p_time time) {

}