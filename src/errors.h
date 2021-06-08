/**
 * Definition of error numbers
 */

#ifndef __ERRORS_H
#define __ERRORS_H

#define MAX_ERROR_NUMBER    10      // Largest (absolute value) of the error number

#define ERR_OUT_OF_MEMORY   -1
#define DEV_ERR_BADDEV      -2      // Device number is bad (too big or no device assigned)
#define DEV_TIMEOUT         -3      // The block device function timed out
#define DEV_CANNOT_INIT     -4      // The block device could not initilize
#define DEV_CANNOT_READ     -5      // The device cannot complete the READ
#define DEV_CANNOT_WRITE    -6      // The device cannot complete the WRITE
#define DEV_BOUNDS_ERR      -7      // The buffer provided is not big enough
#define DEV_NOMEDIA         -8      // There is no media present for that device
#define DEV_WRITEPROT       -9      // The media is write protected
#define ERR_BADCHANNEL      -10     // The channel number was bad

//
// Return the text of an error, given the number.
//
extern const char * error_txt(short error_number);

#endif