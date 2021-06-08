/**
 * Implementation of the PATA hard drive low level driver
 */

#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "constants.h"
#include "trinity.h"
#include "fatfs/ff.h"
#include "dev/block/block.h"
#include "dev/block/pata.h"
#include "dev/channel/text_screen.h"

//
// PATA interface register definitions
//

#define PATA_DATA       ((volatile byte *)0xAFE830) // 8-Bit Access here Only

#define PATA_ERROR      ((volatile byte *)0xAFE831) // Error Information register (only read when there is an error ) - Probably clears Error Bits
#define PATA_ERR_AMNF   0x01    // Error: Address mark not found
#define PATA_ERR_TKZNF  0x02    // Error: Track 0 not found
#define PATA_ERR_ABRT   0x04    // Error: Aborted command
#define PATA_ERR_MCR    0x08    // Error: Media change request
#define PATA_ERR_IDNF   0x10    // Error: ID not found
#define PATA_ERR_MC     0x20    // Error: Media change
#define PATA_ERR_UNC    0x40    // Error: Uncorrectable data error
#define PATA_ERR_BBK    0x80    // Error: Bad block detected

#define PATA_SECT_CNT  ((volatile byte *)0xAFE832)  // Sector Count Register (also used to pass parameter for timeout for IDLE modus Command)
#define PATA_SECT_SRT  ((volatile byte *)0xAFE833)  // Start Sector Register (0 = 256), so start @ 1
#define PATA_CLDR_LO   ((volatile byte *)0xAFE834)  // Low Byte of Cylinder Numnber {7:0}
#define PATA_CLDR_HI   ((volatile byte *)0xAFE835)  //  Hi Byte of Cylinder Number {9:8} (1023-0).
#define PATA_HEAD      ((volatile byte *)0xAFE836)  // Head, device select, {3:0} HEad Number, 4 -> 0:Master, 1:Slave, {7:5} = 101 (legacy)//

// Bit Flags for PATA_CMD_STAT 

//7    6    5   4  3   2   1    0
//BSY DRDY DF DSC DRQ CORR IDX ERR

#define PATA_CMD_STAT  ((volatile byte *)0xAFE837)  // Command/Status Register - Reading this will clear the Interrupt Registers
#define PATA_STAT_BSY   0x80    // BSY (Busy) is set whenever the device has control of the command Block Registers.
#define PATA_STAT_DRDY  0x40    // DRDY (Device Ready) is set to indicate that the device is capable of accepting all command codes.
#define PATA_STAT_DF    0x20    // DF (Device Fault) indicates a device fault error has been detected.
#define PATA_STAT_DSC   0x10    // DSC (Device Seek Complete) indicates that the device heads are settled over a track.
#define PATA_STAT_DRQ   0x08    // DRQ (Data Request) indicates that the device is ready to transfer a word or byte of data between
                                //   the host and the device.
#define PATA_STAT_CORR  0x04    // CORR (Corrected Data) is used to indicate a correctable data error.
#define PATA_STAT_IDX   0x02    // Vendor specific bit
#define PATA_STAT_ERR   0x01    // ERR (Error) indicates that an error occurred during execution of the previous command.

/*
 * - BSY (Busy) is set whenever the device has control of the command
 *   Block Registers. When the BSY bit is equal to one, a write to a
 *   command block register by the host shall be ignored by the
 *   device.
 *
 *   The device shall not change the state of the DRQ bit unless the
 *   BSY bit is equal to one. When the last block of a PIO data in
 *   command has been transferred by the host, then the DRQ bit is
 *   cleared without the BSY bit being set.
 *
 *   When the BSY bit equals zero, the device may only change the IDX,
 *   DRDY, DF, DSC, and CORR bits in the Status register and the Data
 *   register. None of the other command block registers nor other
 *   bits within the Status register shall be changed by the device.
 *
 *   NOTE - BIOSs and software device drivers that sample status as
 *   soon as the BSY bit is cleared to zero may not detect the
 *   assertion of the CORR bit by the device. After the host has
 *   written the Command register either the BSY bit shall be set, or
 *   if the BSY bit is cleared, the DRQ bit shall be set, until
 *   command completion.
 * 
 *   NOTE - The BSY bit is set and then cleared so quickly, that host
 *   detection of the BSY bit being set is not certain.
 *
 *   The BSY bit shall be set by the device under the following
 *   circumstances:
 *     a) within 400 ns after either the negation of RESET- or the
 *        setting of the SRST bit in the Device Control register;
 *     b) within 400 ns after writing the Command register if the DRQ
 *        bit is not set;
 *     c) between blocks of a data transfer during PIO data in
 *        commands if the DRQ bit is not set;
 *     d) after the transfer of a data block during PIO data out
 *        commands if the DRQ bit is not set;
 *     e) during the data transfer of DMA commands if the DRQ bit is
 *        not set.
 *
 *   The device shall not set the BSY bit at any other time.
 *
 * - DRDY (Device Ready) is set to indicate that the device is capable
 *   of accepting all command codes. This bit shall be cleared at
 *   power on. Devices that implement the power management features
 *   shall maintain the DRDY bit equal to one when they are in the
 *   Idle or Standby power modes. When the state of the DRDY bit
 *   changes, it shall not change again until after the host reads the
 *   status register.
 *
 *   When the DRDY bit is equal to zero, a device responds as follows:
 *     a) the device shall accept and attempt to execute the EXECUTE
 *        DEVICE DIAGNOSTIC and INITIALIZE DEVICE PARAMETERS commands;
 *     b) If a device accepts commands other than EXECUTE DEVICE
 *        DIAGNOSTIC and INITIALIZE DEVICE PARAMETERS during the time the
 *        DRDY bit is equal to zero, the results are vendor specific.
 *
 * - DF (Device Fault) indicates a device fault error has been
 *   detected. The internal status or internal conditions that causes
 *   this error to be indicated is vendor specific.
 *
 * - DSC (Device Seek Complete) indicates that the device heads are
 *   settled over a track. When an error occurs, this bit shall not be
 *   changed until the Status register is read by the host, at which
 *   time the bit again indicates the current Seek Complete status.
 *
 * - DRQ (Data Request) indicates that the device is ready to transfer
 *   a word or byte of data between the host and the device.
 *
 * - CORR (Corrected Data) is used to indicate a correctable data
 *   error. The definition of what constitutes a correctable error is
 *   vendor specific. This condition does not terminate a data
 *   transfer.
 *
 * - IDX (Index) is vendor specific.
 *
 * - ERR (Error) indicates that an error occurred during execution of
 *   the previous command. The bits in the Error register have
 *   additional information regarding the cause of the error. Once the
 *   device has set the error bit, the device shall not change the
 *   contents of the following items until a new command has been
 *   accepted, the SRST bit is set to one, or RESET- is asserted: the
 *   ERR bit in the Status register.
 */

#define PATA_DATA_16   ((volatile word *)0xAFE838)  // 16-bit data access

#define PATA_CMD_INIT           0x00
#define PATA_CMD_READ_SECTOR    0x21
#define PATA_CMD_WRITE_SECTOR   0x30
#define PATA_CMD_IDENTITY       0xEC

//
// Variables
//

short g_pata_error = 0;                     // Most recent error code received from the PATA drive
short g_pata_status = PATA_STAT_NOINIT;     // Status of the PATA interface

//
// Code
//

//
// Wait until the PATA drive is no longer busy, or we've run out of time
//
// Returns:
//  0 on success (PATA drive is no longer busy), DEV_TIMEOUT on timeout
//
short pata_wait_not_busy() {
    short count = MAX_TRIES_BUSY;
    byte status;

    TRACE("pata_wait_not_busy");

    do {
        status = *PATA_CMD_STAT;
    } while ((status & PATA_STAT_BSY) && (count-- > 0));

    if (count == 0) {
        return DEV_TIMEOUT;
    } else {
        return 0;
    }
}

//
// Wait until the PATA drive is ready, or we've run out of time
//
// Returns:
//  0 on success (PATA drive is ready), DEV_TIMEOUT on timeout
//
short pata_wait_ready() {
    short count = MAX_TRIES_BUSY;
    byte status;

    TRACE("pata_wait_ready");

    do {
        status = *PATA_CMD_STAT;
    } while (((status & PATA_STAT_DRDY) == 0) && (count-- > 0));

    if (count == 0) {
        return DEV_TIMEOUT;
    } else {
        return 0;
    }
}

//
// Wait until the PATA drive is ready and not busy, or we've run out of time
//
// Returns:
//  0 on success (PATA drive is ready and not busy), DEV_TIMEOUT on timeout
//
short pata_wait_ready_not_busy() {
    short count = MAX_TRIES_BUSY;
    byte status;

    TRACE("pata_wait_ready_not_busy");

    do {
        status = *PATA_CMD_STAT;
    } while (((status & (PATA_STAT_DRDY | PATA_STAT_BSY)) != PATA_STAT_DRDY) && (count-- > 0));

    if (count == 0) {
        return DEV_TIMEOUT;
    } else {
        return 0;
    }
}

//
// Wait until the PATA drive is ready to transfer data, or we've run out of time
//
// Returns:
//  0 on success (PATA drive is ready to transfer data), DEV_TIMEOUT on timeout
//
short pata_wait_data_request() {
    short count = MAX_TRIES_BUSY;
    byte status;

    TRACE("pata_wait_data_request");

    do {
        status = *PATA_CMD_STAT;
    } while (((status & PATA_STAT_DRQ) == 0) && (count-- > 0));

    if (count == 0) {
        return DEV_TIMEOUT;
    } else {
        return 0;
    }
}

byte g_buffer[512];

//
// Identify the PATA drive
//
// Inputs:
//  drive_info = pointer to a s_drive_info
//
// Returns:
//  0 on success, any negative number is an error code
//
short pata_identity(p_drive_info drive_info) {
    byte * buffer;
    word *wptr;
    char * cptr;
    short i;
    short count;
    TRACE("pata_identity");

    *PATA_HEAD = 0xe0;          // Drive 0, lBA enabled, Head 0
    *PATA_SECT_CNT = 1;
    *PATA_SECT_SRT = 0;
    *PATA_CLDR_LO = 0;
    *PATA_CLDR_HI = 0;

    // Issue identity command
    *PATA_CMD_STAT = PATA_CMD_IDENTITY;
    if (pata_wait_not_busy()) {
        return DEV_TIMEOUT;
    }

    // TODO: Wait ~500ns

    if (pata_wait_ready_not_busy()) {
        return DEV_TIMEOUT;
    }

    TRACE("copying data");

    // Copy the data... let the compiler and the FPGA worry about endianess
    wptr = (word *)g_buffer;
    for (i = 0; i < 512; ) {
        word data = *PATA_DATA_16;
        g_buffer[i++] = data & 0xff;
        g_buffer[i++] = (data >> 8) & 0xff;
    }

    TRACE("data copied");

    wptr = (word *)buffer;
    drive_info->flags = g_buffer[1] << 16 | g_buffer[0];
    drive_info->lba_enabled = g_buffer[99] << 16 | g_buffer[98];
    drive_info->l.lbaw.lba_default_lo = g_buffer[121] << 8 | g_buffer[120];
    drive_info->l.lbaw.lba_default_hi = g_buffer[123] << 8 | g_buffer[122];

    // Copy the serial number (need to swap bytes)
    memcpy(&(drive_info->serial_number), g_buffer + 22, sizeof(drive_info->serial_number));

    // Copy the firmware version (need to swap bytes)
    memcpy(&(drive_info->firmware_version), g_buffer + 46, sizeof(drive_info->firmware_version));

    // Copy the model name (need to swap bytes)
    memcpy(&(drive_info->model_name), g_buffer + 54, sizeof(drive_info->model_name));

    return 0;
}

//
// Initialize the PATA hard drive
//
// Returns:
//  0 on success, any negative number is an error code
//
short pata_init() {
    short result;

    TRACE("pata_init");

    // Issue intialize command
    *PATA_CMD_STAT = PATA_CMD_INIT;
    if (pata_wait_not_busy()) {
        return DEV_TIMEOUT;
    }

    *PATA_HEAD = 0xe0;          // Drive 0, lBA enabled, Head 0
    *PATA_SECT_CNT = 1;
    *PATA_SECT_SRT = 0;
    *PATA_CLDR_LO = 0;
    *PATA_CLDR_HI = 0;

    if (pata_wait_ready_not_busy()) {
        return DEV_TIMEOUT;
    }

    // Mark that the drive is initialized and present
    g_pata_status = PATA_STAT_PRESENT;

    return 0;
}

//
// Read a block from the PATA hard drive
//
// Inputs:
//  lba = the logical block address of the block to read
//  buffer = the buffer into which to copy the block data
//  size = the size of the buffer. 
//
// Returns:
//  number of bytes read, any negative number is an error code
//
short pata_read(long lba, byte * buffer, short size) {
    short i;
    word *wptr;
    TRACE("pata_read");

    if (pata_wait_ready_not_busy()) {
        return DEV_TIMEOUT;
    }

    *PATA_HEAD = ((lba >> 24) & 0x07) | 0xe0;       // Upper 3 bits of LBA, Drive 0, LBA mode.
    if (pata_wait_ready_not_busy()) {
        return DEV_TIMEOUT;
    }

    *PATA_SECT_CNT = 1;                             // Read one sector (make this an option?)
    *PATA_SECT_SRT = lba & 0xff;                    // Set the rest of the LBA
    *PATA_CLDR_LO = (lba >> 8) & 0xff;
    *PATA_CLDR_LO = (lba >> 16) & 0xff;

    *PATA_CMD_STAT = PATA_CMD_READ_SECTOR;          // Issue the READ command

    // TODO: Wait ~500ns

    if (pata_wait_ready_not_busy()) {
        return DEV_TIMEOUT;
    }

    // Copy the data... let the compiler and the FPGA worry about endianess
    for (i = 0, wptr = (word *)buffer; i < size; i += 2) {
        *wptr++ = *PATA_DATA_16;
    }

    return 0;
}

//
// Write a block to the PATA hard drive
//
// Inputs:
//  lba = the logical block address of the block to write
//  buffer = the buffer containing the data to write
//  size = the size of the buffer. 
//
// Returns:
//  number of bytes written, any negative number is an error code
//
short pata_write(long lba, const byte * buffer, short size) {
    short i;
    word *wptr;
    TRACE("pata_write");

    if (pata_wait_ready_not_busy()) {
        return DEV_TIMEOUT;
    }

    *PATA_HEAD = ((lba >> 24) & 0x07) | 0xe0;       // Upper 3 bits of LBA, Drive 0, LBA mode.
    if (pata_wait_ready_not_busy()) {
        return DEV_TIMEOUT;
    }

    *PATA_SECT_CNT = 1;                             // Read one sector (make this an option?)
    *PATA_SECT_SRT = lba & 0xff;                    // Set the rest of the LBA
    *PATA_CLDR_LO = (lba >> 8) & 0xff;
    *PATA_CLDR_LO = (lba >> 16) & 0xff;

    *PATA_CMD_STAT = PATA_CMD_WRITE_SECTOR;         // Issue the WRITE command

    // TODO: Wait ~500ns

    if (pata_wait_ready_not_busy()) {
        return DEV_TIMEOUT;
    }

    // Copy the data... let the compiler and the FPGA worry about endianess
    for (i = 0, wptr = (word *)buffer; i < size; i += 2) {
        *PATA_DATA_16 = *wptr++;
    }

    return 0;
}

//
// Return the status of the PATA hard drive
//
// Returns:
//  the status of the device
//
short pata_status() {
    TRACE("pata_status");
    return g_pata_status;
}

//
// Return any error code of the PATA hard drive
//
// Returns:
//  the error code of the device
//
short pata_error() {
    TRACE("pata_error");
    return g_pata_error;
}

//
// Ensure that any pending writes to teh device have been completed
//
// Returns:
//  0 on success, any negative number is an error code
//
short pata_flush() {
    TRACE("pata_flush");
    return 0;
}

//
// Issue a control command to the PATA hard drive
//
// Inputs:
//  command = the number of the command to send
//  buffer = pointer to bytes of additional data for the command
//  size = the size of the buffer
//
// Returns:
//  0 on success, any negative number is an error code
//
short pata_ioctrl(short command, byte * buffer, short size) {
    short result;
    DWORD *p_dword;
    WORD *p_word;
    LBA_t *p_lba_word;
    t_drive_info drive_info;
    p_drive_info p_info;

    TRACE("pata_ioctrl");

    switch (command) {
        case PATA_GET_SECTOR_COUNT:
            p_lba_word = (LBA_t *)buffer;
            result = pata_identity(&drive_info);
            if (result != 0) {
                return result;
            }

            *p_lba_word = drive_info.l.lba_default;
            break;

        case PATA_GET_SECTOR_SIZE:
            // Return the size of a sector... always 512
            p_word = (WORD *)buffer;
            *p_word = PATA_SECTOR_SIZE;
            break;

        case PATA_GET_BLOCK_SIZE:
            // This isn't a flash device... return 1
            p_dword = (DWORD *)buffer;
            *p_dword = 1;
            break;

        case PATA_GET_DRIVE_INFO:
            p_info = (p_drive_info *)buffer;
            result = pata_identity(p_info);
            if (result != 0) {
                return result;
            }
            break;      

        default:
            return 0;
    }

    return 0;
}

//
// Install the PATA driver
//
// Returns:
//  0 on success, any negative number is an error code
//
short pata_install() {
    t_dev_block bdev;

    TRACE("pata_install");

    g_pata_error = 0;
    g_pata_status = PATA_STAT_NOINIT;

    // Check if drive is installed
    if ((*DIP_BOOTMODE & HD_INSTALLED) == 0) {
        bdev.number = BDEV_HDC;
        bdev.name = "HDD";
        bdev.init = pata_init;
        bdev.read = pata_read;
        bdev.write = pata_write;
        bdev.status = pata_status;
        bdev.flush = pata_flush;
        bdev.ioctrl = pata_ioctrl;

        g_pata_status = PATA_STAT_PRESENT & PATA_STAT_NOINIT;

        return bdev_register(&bdev);
    } else {
        return 0;
    }
} 