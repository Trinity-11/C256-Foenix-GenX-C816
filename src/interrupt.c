//
// Code for managing the interupts
//

#include "vicky_general.h"
#include "types.h"
#include "interrupt.h"
#include "dev/channel/text_screen.h"

//
// Storage for the interrupt handler pointers
//

IRQ_HANDLER g_irq_handlers[MAX_IRQ_INTERRUPTS][MAX_IRQ_BLOCKS];

//
// Initialize the IRQ settings
//
void irq_init() {
    volatile byte *int_edge = (byte *)INT_EDGE_REG0;
    volatile byte *int_mask = (byte *)INT_MASK_REG0;
    volatile byte *int_pending = (byte *)INT_PENDING_REG0;
    short block;
    short irq_num;

    for (block = 0; block < MAX_IRQ_BLOCKS; block++) {
        int_mask[block] = 0xff;     // Mask off all interrupts
        int_edge[block] = 0xff;     // Set all interrupts to falling edge
        int_pending[block] = 0xff;  // Clear any pending interrupts in this block
        
        for (irq_num = 0; irq_num < MAX_IRQ_INTERRUPTS; irq_num++) {
            // Make sure we have no handlers registered for these interrupts
            // Handlers the kernel or devices need will be registered later
            g_irq_handlers[irq_num][block] = 0;
        }
    }

    irq_enable_sys();
}

//
// Enable system-wide IRQ processing
//
void irq_enable_sys() {
    asm cli;
}

//
// Disable system-wide IRQ processing
//
void irq_disable_sys() {
    asm sei;
}

//
// Enable processing of an interrupt
//
// Inputs:
//      irq_num = the number of the interrupt
//
void irq_enable(short irq_num) {
    volatile byte *int_mask = (byte *)INT_MASK_REG0;
    byte mask = 1;
    short block = (irq_num >> 3) & 0x03;
    irq_num = irq_num & 0x03;

    if (block < MAX_IRQ_BLOCKS) {
        if (irq_num < MAX_IRQ_INTERRUPTS) {
            mask = ~(mask << irq_num);
            int_mask[block] = int_mask[block] & mask;
        }
    }
}

//
// Disable processing of an interrupt
//
// Inputs:
//      irq_num = the number of the interrupt
//
void irq_disable(short irq_num) {
    volatile byte *int_mask = (byte *)INT_MASK_REG0;
    byte mask = 1;
    short block = (irq_num >> 3) & 0x03;
    irq_num = irq_num & 0x03;

    if (block < MAX_IRQ_BLOCKS) {
        if (irq_num < MAX_IRQ_INTERRUPTS) {
            mask = mask << irq_num;
            int_mask[block] = int_mask[block] | mask;
        }
    }
}

//
// Acknowledge an interrupt to the interrupt controller and clear it from the pending register
//
// Inputs:
//      irq_num = the number of the interrupt
//
extern void irq_clear(short irq_num) {
    volatile byte *int_mask = (byte *)INT_MASK_REG0;
    byte mask = 1;
    short block = (irq_num >> 3) & 0x03;
    irq_num = irq_num & 0x03;


    if (block < MAX_IRQ_BLOCKS) {
        if (irq_num < MAX_IRQ_INTERRUPTS) {
            mask = mask << irq_num;
            int_mask[block] = int_mask[block] & mask;
        }
    }  
}

//
// Register a FAR subroutine to be the handler for an interrupt.
//
// Inputs:
//      irq_num = the number of the interrupt
//      handler = far pointer to the interrupt handler function
//
void irq_register(short irq_num, IRQ_HANDLER handler) {
    short block = (irq_num >> 3) & 0x03;
    irq_num = irq_num & 0x03;

    if (block < MAX_IRQ_BLOCKS) {
        if (irq_num < MAX_IRQ_INTERRUPTS) {
            irq_disable_sys();
            g_irq_handlers[irq_num][block] = handler;
            irq_enable_sys();
        }
    }  
}

//
// Remove the registration of an IRQ handler
//
// Inputs:
//      irq_num = the number of the interrupt
//
void irq_deregister(short irq_num) {
    short block = (irq_num >> 3) & 0x03;
    irq_num = irq_num & 0x03;

    if (block < MAX_IRQ_BLOCKS) {
        if (irq_num < MAX_IRQ_INTERRUPTS) {
            irq_disable_sys();
            g_irq_handlers[irq_num][block] = 0;
            irq_enable_sys();
        }
    } 
}

//
// Master routine for processing interrupts... scans the pending list in priority order
// and calls associated handlers
//
// This routine is to be called by the system dependant interrupt handler ONLY!
//
void irq_dispatch() {
    volatile byte *int_pending = (byte *)INT_PENDING_REG0;
    volatile byte *int_mask = (byte *)INT_MASK_REG0;
    short block;
    short irq_num;

    // For each interrupt block...
    for (block = 0; block < MAX_IRQ_BLOCKS; block++) {
        // Get that block's list of pending interrupts
        byte pending = int_pending[block];
        byte irq_mask = 1;
        if (pending != 0) {
            // If there is anything pending for this block...

            // For each interrupt in the block
            for (irq_num = 0; irq_num < MAX_IRQ_INTERRUPTS; irq_num++, irq_mask << 1) {
                if (pending & irq_mask) {
                    // There is an interrupt pending for this number, so try to get its handler
                    IRQ_HANDLER handler = g_irq_handlers[irq_num][block];
                    if (handler != 0) {
                        // If there is one, call it
                        handler();
                    }

                    // Acknowledge the interrupt
                    int_pending[block] = irq_mask; 
                }
            }
        }
    }
}
