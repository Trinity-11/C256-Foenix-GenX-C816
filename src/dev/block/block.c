/**
 * Implementation of block device routines
 */

#include "dev/block/block.h"

t_dev_block g_block_devs[BDEV_DEVICES_MAX];

//
// Initialize the block driver system
//
void bdev_init_system() {
    int i;
    for (i = 0; i < BDEV_DEVICES_MAX; i++) {
        g_block_devs[i].number = 0;
        g_block_devs[i].name = 0;
    }
}

//
// Register a block device driver
//
short bdev_register(p_dev_block device) {
    short dev;

    dev = device->number;
    if (dev < BDEV_DEVICES_MAX) {
        // Copy the device description into the master table

        p_dev_block bdev = &g_block_devs[dev];
        bdev->number = device->number;
        bdev->name = device->name;
        bdev->init = device->init;
        bdev->read = device->read;
        bdev->write = device->write;
        bdev->status = device->status;
        bdev->flush = device->flush;
        bdev->ioctrl = device->ioctrl;
        return 0;
    } else {
        return DEV_ERR_BADDEV;
    }
}

//
// Initialize the device
//
// Inputs:
//  dev = the number of the device
//
// Returns:
//  0 on success, any negative number is an error code
//
short bdev_init(short dev)  {
    if (dev < BDEV_DEVICES_MAX) {
        p_dev_block bdev = &g_block_devs[dev];
        if (bdev->number == dev) {
            return bdev->init();
        } else {
            return DEV_ERR_BADDEV;
        }
    }
}

//
// Read a block from the device
//
// Inputs:
//  dev = the number of the device
//  lba = the logical block address of the block to read
//  buffer = the buffer into which to copy the block data
//  size = the size of the buffer. 
//
// Returns:
//  number of bytes read, any negative number is an error code
//
short bdev_read(short dev, long lba, byte * buffer, short size) {
    if (dev < BDEV_DEVICES_MAX) {
        p_dev_block bdev = &g_block_devs[dev];
        if (bdev->number == dev) {
            return bdev->read(lba, buffer, size);
        } else {
            return DEV_ERR_BADDEV;
        }
    }    
}

//
// Write a block from the device
//
// Inputs:
//  dev = the number of the device
//  lba = the logical block address of the block to write
//  buffer = the buffer containing the data to write
//  size = the size of the buffer. 
//
// Returns:
//  number of bytes written, any negative number is an error code
//
short bdev_write(short dev, long lba, const byte * buffer, short size) {
    if (dev < BDEV_DEVICES_MAX) {
        p_dev_block bdev = &g_block_devs[dev];
        if (bdev->number == dev) {
            return bdev->write(lba, buffer, size);
        } else {
            return DEV_ERR_BADDEV;
        }
    }    
}

//
// Return the status of the block device
//
// Inputs:
//  dev = the number of the device
//
// Returns:
//  the status of the device
//
short bdev_status(short dev) {
    if (dev < BDEV_DEVICES_MAX) {
        p_dev_block bdev = &g_block_devs[dev];
        if (bdev->number == dev) {
            return bdev->status();
        } else {
            return DEV_ERR_BADDEV;
        }
    }    
}

//
// Ensure that any pending writes to teh device have been completed
//
// Inputs:
//  dev = the number of the device
//
// Returns:
//  0 on success, any negative number is an error code
//
short bdev_flush(short dev) {
    if (dev < BDEV_DEVICES_MAX) {
        p_dev_block bdev = &g_block_devs[dev];
        if (bdev->number == dev) {
            return bdev->flush();
        } else {
            return DEV_ERR_BADDEV;
        }
    }    
}

//
// Issue a control command to the device
//
// Inputs:
//  dev = the number of the device
//  command = the number of the command to send
//  buffer = pointer to bytes of additional data for the command
//  size = the size of the buffer
//
// Returns:
//  0 on success, any negative number is an error code
//
short bdev_ioctrl(short dev, short command, byte * buffer, short size) {
    if (dev < BDEV_DEVICES_MAX) {
        p_dev_block bdev = &g_block_devs[dev];
        if (bdev->number == dev) {
            return bdev->ioctrl(command, buffer, size);
        } else {
            return DEV_ERR_BADDEV;
        }
    }     
}