/**
 * Implementation of the SDC device driver
 */

#include "debug.h"

#include "fatfs/ff.h"
#include "constants.h"
#include "gabe_general.h"
#include "dev/block/block.h"
#include "dev/block/sdc.h"
#include "dev/channel/text_screen.h"

byte g_sdc_status = SDC_STAT_NOINIT;
byte g_sdc_error = 0;

//
// Attempt to reset the SD controller
//
void sdc_reset() {
    short delay;

    TRACE("sdc_reset");

    // NOTE: I haven't quite figured out reset on this block... setting the reset bit
    // seems to leave the controller in an odd state.

    // *SDC_CONTROL_REG = 1;
}

//
// Return true if there is an SD card in the slot
//
short sdc_detected() {
    return (*SDCARD_STAT & SDC_DETECTED) != SDC_DETECTED;
}

//
// Return true if there is an SD card is protected
//
short sdc_protected() {
    return (*SDCARD_STAT & SDC_WRITEPROT) != SDC_WRITEPROT;
}

//
// Turn the SDC LED on or off
//
// Inputs:
//  is_on = if 0, turn the LED off, otherwise turn the LED on
//
void sdc_set_led(short is_on) {
    volatile byte *gabe_control = (byte *)GABE_MSTR_CTRL;

    if (is_on) {
        *gabe_control = *gabe_control | GABE_CTRL_SDC_LED;
    } else {
        *gabe_control = *gabe_control & ~GABE_CTRL_SDC_LED;
    }
}

//
// Wait for the SDC to complete its transaction
//
// Returns:
//  0 on success, DEV_TIMEOUT on timeout
//
short sdc_wait_busy() {
    short retry_count = MAX_TRIES_BUSY;
    byte status;

    do {
        if (retry_count-- == 0) {
            // If we have run out of tries, return a TIMEOUT error
            return DEV_TIMEOUT;
        }
        status = *SDC_TRANS_STATUS_REG;
    } while ((status & SDC_TRANS_BUSY) == SDC_TRANS_BUSY);

    return 0;
}

//
// Initialize the SDC
//
// Returns:
//  0 on success, any negative number is an error code
//
short sdc_init() {
    TRACE("sdc_init");

    // Check for presence of the card

    if (!sdc_detected()) {
        // SDC_DETECTED is active 0... 1 means there is no card
        g_sdc_status = SDC_STAT_NOINIT;
        return DEV_NOMEDIA;
    }

    *SDC_TRANS_TYPE_REG = SDC_TRANS_INIT_SD;        // Start the INIT_SD transaction
    *SDC_TRANS_CONTROL_REG = SDC_TRANS_START;

    if (sdc_wait_busy() == 0) {                     // Wait for it to complete
        g_sdc_error = *SDC_TRANS_ERROR_REG;         // Check for any error condition
        if (g_sdc_error == 0) {
            TRACE("sdc_init: SUCCESS");
            g_sdc_status = 0;                       // Flag that the SD has been initialized
            return 0;
        } else {
            TRACE("sdc_init: DEV_CANNOT_INIT");
            g_sdc_status = SDC_STAT_NOINIT;
            return DEV_CANNOT_INIT;
        }
    } else {
        TRACE("sdc_init: DEV_TIMEOUT");
        g_sdc_status = SDC_STAT_NOINIT;
        return DEV_TIMEOUT;
    }
}

//
// Read a block from the SDC
//
// Inputs:
//  lba = the logical block address of the block to read
//  buffer = the buffer into which to copy the block data
//  size = the size of the buffer. 
//
// Returns:
//  number of bytes read, any negative number is an error code
//
short sdc_read(long lba, byte * buffer, short size) {
    long adjusted_lba;

    TRACE("sdc_read");

    // Check for presence of the card

    if (!sdc_detected()) {
        // SDC_DETECTED is active 0... 1 means there is no card
        g_sdc_status = SDC_STAT_NOINIT;
        return DEV_NOMEDIA;
    }

    sdc_set_led(1);                         // Turn on the SDC LED

    // Send the LBA to the SDC

    adjusted_lba = lba << 9;
    *SDC_SD_ADDR_7_0_REG = adjusted_lba & 0xff;
    *SDC_SD_ADDR_15_8_REG = (adjusted_lba >> 8) & 0xff;
    *SDC_SD_ADDR_23_16_REG = (adjusted_lba >> 16) & 0xff;
    *SDC_SD_ADDR_31_24_REG = (adjusted_lba >> 24) & 0xff;

    // Start the READ transaction

    *SDC_TRANS_TYPE_REG = SDC_TRANS_READ_BLK;   // Set the transaction type to READ
    *SDC_TRANS_CONTROL_REG = SDC_TRANS_START;   // Start the transaction

    if (sdc_wait_busy() == 0) {                 // Wait for the transaction to complete
        g_sdc_error = *SDC_TRANS_ERROR_REG;     // Check for errors

        if (g_sdc_error != 0) {
            sdc_set_led(0);                     // Turn off the SDC LED
            return DEV_CANNOT_READ;

        } else {
            short count;
            short i;

            // Get the number of bytes to be read and make sure there is room
            count = *SDC_RX_FIFO_DATA_CNT_HI << 8 | *SDC_RX_FIFO_DATA_CNT_LO;
            if (count > size) {
                return DEV_BOUNDS_ERR;
            }

            for (i = 0; i < count; i++) {        // Fetch the bytes from the SDC
                buffer[i] = *SDC_RX_FIFO_DATA_REG;
            }

            sdc_set_led(0);                     // Turn off the SDC LED

            g_sdc_error = *SDC_TRANS_ERROR_REG; // Check for errors
            if (g_sdc_error != 0) {
                return DEV_CANNOT_READ;
            } else {
                // Success: return the byte count
                return count;
            }
        }
    } else {
        sdc_set_led(0);                         // Turn off the SDC LED
        return DEV_TIMEOUT;
    }
}

//
// Write a block to the SDC
//
// Inputs:
//  lba = the TRACEical block address of the block to write
//  buffer = the buffer containing the data to write
//  size = the size of the buffer. 
//
// Returns:
//  number of bytes written, any negative number is an error code
//
short sdc_write(long lba, const byte * buffer, short size) {
    long adjusted_lba;
    short i;

    TRACE("sdc_write");

    // Check for presence of the card

    if (!sdc_detected()) {
        // SDC_DETECTED is active 0... 1 means there is no card
        g_sdc_status = SDC_STAT_NOINIT;
        return DEV_NOMEDIA;
    }

    sdc_set_led(1);                         // Turn on the SDC LED

    if (size <= SDC_SECTOR_SIZE) {
        // Copy the data to the SDC, if there isn't too much...
        for (i = 0; i < size; i++) {
            *SDC_TX_FIFO_DATA_REG = buffer[i];
        }

        if (size < SDC_SECTOR_SIZE) {
            // If we copied less than a block's worth, pad the rest with 0s...
            for (i = 0; i < SDC_SECTOR_SIZE - size; i++) {
                *SDC_TX_FIFO_DATA_REG = 0;
            }
        }
    
    } else {
        // If size is too big, return a BOUNDS error
        return DEV_BOUNDS_ERR;
    }

    // Send the LBA to the SDC

    adjusted_lba = lba << 9;
    *SDC_SD_ADDR_7_0_REG = adjusted_lba & 0xff;
    *SDC_SD_ADDR_15_8_REG = (adjusted_lba >> 8) & 0xff;
    *SDC_SD_ADDR_23_16_REG = (adjusted_lba >> 16) & 0xff;
    *SDC_SD_ADDR_31_24_REG = (adjusted_lba >> 24) & 0xff;

    // Start the WRITE transaction

    *SDC_TRANS_TYPE_REG = SDC_TRANS_WRITE_BLK;  // Set the transaction type to WRITE
    *SDC_TRANS_CONTROL_REG = SDC_TRANS_START;   // Start the transaction

    if (sdc_wait_busy() == 0) {                 // Wait for the transaction to complete
        g_sdc_error = *SDC_TRANS_ERROR_REG;     // Check for errors

        if (g_sdc_error != 0) {
            sdc_set_led(0);                     // Turn off the SDC LED
            return DEV_CANNOT_WRITE;

        } else {
            // Success: return the byte count
            return size;
        }
    } else {
        sdc_set_led(0);                         // Turn off the SDC LED
        return DEV_TIMEOUT;
    }
}

//
// Return the status of the SDC
//
// Returns:
//  the status of the device
//
short sdc_status() {
    short status = g_sdc_status;

    if (sdc_detected()) {
        // Add the PRESENT flag, if the card is inserted
        status |= SDC_STAT_PRESENT;
    }

    if (sdc_protected()) {
        // Add the PROTECTED flag, if the card is write-protected
        status |= SDC_STAT_PROTECTED;
    }
    
    return status;
}

//
// Return any error code of the SDC
//
// Returns:
//  the error code of the device
//
short sdc_error() {
    return g_sdc_error;
}

//
// Ensure that any pending writes to teh device have been completed
//
// Returns:
//  0 on success, any negative number is an error code
//
short sdc_flush() {
    return 0;           // We don't buffer writes... always return success
}

//
// Return the count of sectors in this SD card
//
short sdc_sector_count() {
    // TODO: implement this!
    return 1000;
}

#define SDC_GET_SECTOR_COUNT 1
#define SDC_GET_SECTOR_SIZE 2
#define SDC_GET_BLOCK_SIZE 3

//
// Issue a control command to the device
//
// Inputs:
//  command = the number of the command to send
//  buffer = pointer to bytes of additional data for the command
//  size = the size of the buffer
//
// Returns:
//  0 on success, any negative number is an error code
//
short sdc_ioctrl(short command, byte * buffer, short size) {
    DWORD *p_dword;
    WORD *p_word;
    LBA_t *p_lba_word;

    switch (command) {
        case SDC_GET_SECTOR_COUNT:
            p_lba_word = (LBA_t *)buffer;
            *p_lba_word = sdc_sector_count();
            break;

        case SDC_GET_SECTOR_SIZE:
            // Return the size of a sector... always 512
            p_word = (WORD *)buffer;
            *p_word = SDC_SECTOR_SIZE;
            break;

        case SDC_GET_BLOCK_SIZE:
            // We don't know what the block size is... return 1
            p_dword = (DWORD *)buffer;
            *p_dword = 1;
            break;

        default:
            return 0;
    }
}

//
// Install the SDC driver
//
short sdc_install() {
    t_dev_block dev;                    // bdev_register copies the data, so we'll allocate this on the stack

    TRACE("sdc_install");

    sdc_reset();

    dev.number = BDEV_SDC;
    dev.name = "SDC";
    dev.init = sdc_init;
    dev.read = sdc_read;
    dev.write = sdc_write;
    dev.flush = sdc_flush;
    dev.status = sdc_status;
    dev.ioctrl = sdc_ioctrl;

    return bdev_register(&dev);
}