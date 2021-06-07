/**
 * Definitions support low level SDC device driver
 */

#ifndef __SDC_H
#define __SDC_H

#include "types.h"

//
// Definitions for GABE's internal SD card controller
//

#define SDCARD_STAT             ((volatile byte *)0xAFE812)     // (R) SDCARD (Bit[0] = CD, Bit[1] = WP)
#define SDC_DETECTED            0x01        // SD card has been detected (0 = card present, 1 = no card present)
#define SDC_WRITEPROT           0x02        // SD card is write protected (0 = card is writeable, 1 = card is write protected or missing)

#define SDC_VERSION_REG         ((volatile byte *)0xAFEA00)     // Ought to read 12
#define SDC_CONTROL_REG         ((volatile byte *)0xAFEA01)     // Bit0 1 = Reset core logic, and registers. Self clearing
#define SDC_TRANS_TYPE_REG      ((volatile byte *)0xAFEA02)     // Bit[1:0]:
#define SDC_TRANS_DIRECT        0x00        // 00 = Direct Access
#define SDC_TRANS_INIT_SD       0x01        // 01 = Init SD
#define SDC_TRANS_READ_BLK      0x02        // 10 = RW_READ_BLOCK (512 Bytes)
#define SDC_TRANS_WRITE_BLK     0x03        // 11 = RW_WRITE_SD_BLOCK

#define SDC_TRANS_CONTROL_REG   ((volatile byte *)0xAFEA03)
#define SDC_TRANS_START         0x01

#define SDC_TRANS_STATUS_REG    ((volatile byte *)0xAFEA04)
#define SDC_TRANS_BUSY          0x01        //  1= Transaction Busy

#define SDC_TRANS_ERROR_REG     ((volatile byte *)0xAFEA05)
#define SDC_TRANS_INIT_NO_ERR   0x00        // Init Error Report [1:0]
#define SDC_TRANS_INIT_CMD0_ERR 0x01
#define SDC_TRANS_INIT_CMD1_ERR 0x02

#define SDC_TRANS_RD_NO_ERR     0x00        // Read Error Report [3:2]
#define SDC_TRANS_RD_CMD_ERR    0x04
#define SDC_TRANS_RD_TOKEN_ERR  0x08

#define SDC_TRANS_WR_NO_ERR     0x00        // Write Report Error  [5:4]
#define SDC_TRANS_WR_CMD_ERR    0x10
#define SDC_TRANS_WR_DATA_ERR   0x20
#define SDC_TRANS_WR_BUSY_ERR   0x30

#define SDC_DIRECT_ACCESS_REG   ((volatile byte *)0xAFEA06) // SPI Direct Read and Write - Set DATA before initiating direct Access Transaction

#define SDC_SD_ADDR_7_0_REG     ((volatile byte *)0xAFEA07) // Set the ADDR before a block read or block write
#define SDC_SD_ADDR_15_8_REG    ((volatile byte *)0xAFEA08) // Addr0 [8:0] Always should be 0, since each block is 512Bytes
#define SDC_SD_ADDR_23_16_REG   ((volatile byte *)0xAFEA09)
#define SDC_SD_ADDR_31_24_REG   ((volatile byte *)0xAFEA0A)

#define SDC_SPI_CLK_DEL_REG     ((volatile byte *)0xAFEA0B)

#define SDC_RX_FIFO_DATA_REG    ((volatile byte *)0xAFEA10) // Data from the Block Read
#define SDC_RX_FIFO_DATA_CNT_HI ((volatile byte *)0xAFEA12) // How many Bytes in the FIFO HI
#define SDC_RX_FIFO_DATA_CNT_LO ((volatile byte *)0xAFEA13) // How many Bytes in the FIFO LO
#define SDC_RX_FIFO_CTRL_REG    ((volatile byte *)0xAFEA14) // Bit0  Force Empty - Set to 1 to clear FIFO, self clearing (the bit)

#define SDC_TX_FIFO_DATA_REG    ((volatile byte *)0xAFEA20) // Write Data Block here
#define SDC_TX_FIFO_CTRL_REG    ((volatile byte *)0xAFEA24) // Bit0  Force Empty - Set to 1 to clear FIFO, self clearing (the bit)

#define SDC_SECTOR_SIZE         512         // Size of a block on the SDC

#define SDC_STAT_NOINIT         0x01        // SD has not been initialized
#define SDC_STAT_PRESENT        0x02        // SD is present
#define SDC_STAT_PROTECTED      0x04        // SD is write-protected

//
// Install the SDC driver
//
extern short sdc_install();

//
// Initialize the SDC
//
// Returns:
//  0 on success, any negative number is an error code
//
extern short sdc_init();

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
extern short sdc_read(long lba, byte * buffer, short size);

//
// Write a block to the SDC
//
// Inputs:
//  lba = the logical block address of the block to write
//  buffer = the buffer containing the data to write
//  size = the size of the buffer. 
//
// Returns:
//  number of bytes written, any negative number is an error code
//
extern short sdc_write(long lba, const byte * buffer, short size);

//
// Return the status of the SDC
//
// Returns:
//  the status of the device
//
extern short sdc_status();

//
// Return any error code of the SDC
//
// Returns:
//  the error code of the device
//
extern short sdc_error();

//
// Ensure that any pending writes to teh device have been completed
//
// Returns:
//  0 on success, any negative number is an error code
//
extern short sdc_flush();

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
extern short sdc_ioctrl(short command, byte * buffer, short size);

#endif