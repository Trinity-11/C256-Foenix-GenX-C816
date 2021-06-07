/**
 * Definition of error numbers
 */

#ifndef __ERRORS_H
#define __ERRORS_H

#define MAX_ERROR_NUMBER    9   // Largest (absolute value) of the error number

#define DEV_ERR_BADDEV      -1  // Device number is bad (too big or no device assigned)
#define DEV_TIMEOUT         -2  // The block device function timed out
#define DEV_CANNOT_INIT     -3  // The block device could not initilize
#define DEV_CANNOT_READ     -4  // The device cannot complete the READ
#define DEV_CANNOT_WRITE    -5  // The device cannot complete the WRITE
#define DEV_BOUNDS_ERR      -6  // The buffer provided is not big enough
#define DEV_NOMEDIA         -7  // There is no media present for that device
#define DEV_WRITEPROT       -8  // The media is write protected
#define ERR_BADCHANNEL      -9  // The channel number was bad

//
// Return the text of an error, given the number.
//
extern const char * error_txt(short error_number);

#endif