/**
 * Implementation of error code
 */

#include "errors.h"

char * g_error_names[] = {
    "Out of memory",
    "Device number is bad",
    "Operation timed out",
    "Device could not be initialized",
    "Unable to read from device",
    "Unable to write to device",
    "Bounds error",
    "No media is present",
    "Device is write protected",
    "Bad channel number"
};

//
// Return the text of an error, given the number.
//
const char * error_txt(short error_number) {
    short index = (error_number < 0) ? -1 * error_number : error_number;
    if (index < MAX_ERROR_NUMBER) {
        return g_error_names[index];
    } else {
        return "Unknown error.";
    }
}