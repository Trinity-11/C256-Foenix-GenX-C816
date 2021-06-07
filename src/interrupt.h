//
// Definitions for interrupts
//

#ifndef __INTERRUPT_H
#define __INTERRUPT_H

#define MAX_IRQ_BLOCKS          4
#define MAX_IRQ_INTERRUPTS      8

// Pending Interrupt (Read and Write Back to Clear) Registers

#define INT_PENDING_REG0        0x000140
#define INT_PENDING_REG1        0x000141
#define INT_PENDING_REG2        0x000142
#define INT_PENDING_REG3        0x000143

// Interrupt Polarity Set Registers

#define INT_POL_REG0            0x000144
#define INT_POL_REG1            0x000145
#define INT_POL_REG2            0x000146
#define INT_POL_REG3            0x000147

// Interrupt Edge Detection Enable Registers

#define INT_EDGE_REG0           0x000148
#define INT_EDGE_REG1           0x000149
#define INT_EDGE_REG2           0x00014A
#define INT_EDGE_REG3           0x00014B

// Interrupt Mask Registers

#define INT_MASK_REG0           0x00014C
#define INT_MASK_REG1           0x00014D
#define INT_MASK_REG2           0x00014E
#define INT_MASK_REG3           0x00014F

// Interrupt Bit Definition

// Register Block 0

#define FNX0_INT00_SOF          0x01  //Start of Frame @ 60FPS
#define FNX0_INT01_SOL          0x02  //Start of Line (Programmable)
#define FNX0_INT02_TMR0         0x04  //Timer 0 Interrupt
#define FNX0_INT03_TMR1         0x08  //Timer 1 Interrupt
#define FNX0_INT04_TMR2         0x10  //Timer 2 Interrupt
#define FNX0_INT05_RTC          0x20  //Real-Time Clock Interrupt
#define FNX0_INT06_FDC          0x40  //Floppy Disk Controller
#define FNX0_INT07_MOUSE        0x80  // Mouse Interrupt (INT12 in SuperIO IOspace)

// Register Block 1

#define FNX1_INT00_KBD          0x01  //Keyboard Interrupt
#define FNX1_INT01_COL0         0x02  //VICKY_II (INT2) Sprite Collision 
#define FNX1_INT02_COL1         0x04  //VICKY_II (INT3) Bitmap Collision
#define FNX1_INT03_COM2         0x08  //Serial Port 2
#define FNX1_INT04_COM1         0x10  //Serial Port 1
#define FNX1_INT05_MPU401       0x20  //Midi Controller Interrupt
#define FNX1_INT06_LPT          0x40  //Parallel Port
#define FNX1_INT07_SDCARD       0x80  //SD Card Controller Interrupt (CH376S)

// Register Block 2

#define FNX2_INT00_OPL3         0x01  //OPl3
#define FNX2_INT01_GABE_INT0    0x02  //GABE (INT0) - TBD
#define FNX2_INT02_GABE_INT1    0x04  //GABE (INT1) - TBD
#define FNX2_INT03_VDMA         0x08  //VICKY_II (INT4) - VDMA Interrupt
#define FNX2_INT04_COL2         0x10  //VICKY_II (INT5) Tile Collision
#define FNX2_INT05_GABE_INT2    0x20  //GABE (INT2) - TBD
#define FNX2_INT06_EXT          0x40  //External Expansion
#define FNX2_INT07_SDCARD_INS   0x80  // SDCARD Insertion

// Register Block 3 (FMX Expansion)

#define FNX3_INT00_OPN2         0x01  //OPN2
#define FNX3_INT01_OPM          0x02  //OPM
#define FNX3_INT02_IDE          0x04  //HDD IDE INTERRUPT
#define FNX3_INT03_TBD          0x08  //TBD
#define FNX3_INT04_TBD          0x10  //TBD
#define FNX3_INT05_TBD          0x20  //GABE (INT2) - TBD
#define FNX3_INT06_TBD          0x40  //External Expansion
#define FNX3_INT07_TBD          0x80  // SDCARD Insertion

// Interrupt number definitions

#define IRQ00_SOF               000     //Start of Frame @ 60FPS
#define IRQ01_SOL               001     //Start of Line (Programmable)
#define IRQ02_TMR0              002     //Timer 0 Interrupt
#define IRQ03_TMR1              003     //Timer 1 Interrupt
#define IRQ04_TMR2              004     //Timer 2 Interrupt
#define IRQ05_RTC               005     //Real-Time Clock Interrupt
#define IRQ06_FDC               006     //Floppy Disk Controller
#define IRQ07_MOUSE             007     // Mouse Interrupt (INT12 in SuperIO IOspace)
#define IRQ08_KBD               010     //Keyboard Interrupt
#define IRQ09_COL0              011     //VICKY_II (INT2) Sprite Collision 
#define IRQ10_COL1              012     //VICKY_II (INT3) Bitmap Collision
#define IRQ11_COM2              013     //Serial Port 2
#define IRQ12_COM1              014     //Serial Port 1
#define IRQ13_MPU401            015     //Midi Controller Interrupt
#define IRQ14_LPT               016     //Parallel Port
#define IRQ15_SDCARD            017     //SD Card Controller Interrupt (CH376S... unused)
#define IRQ16_OPL3              020     //OPl3
#define IRQ17_GABE_INT0         021     //GABE (INT0) - TBD
#define IRQ18_GABE_INT1         022     //GABE (INT1) - TBD
#define IRQ19_VDMA              023     //VICKY_II (INT4) - VDMA Interrupt
#define IRQ20_COL2              024     //VICKY_II (INT5) Tile Collision
#define IRQ21_GABE_INT2         025     //GABE (INT2) - TBD
#define IRQ22_EXT               026     //External Expansion
#define IRQ23_SDCARD_INS        027     // SDCARD Insertion
#define IRQ24_OPN2              030     //OPN2
#define IRQ25_OPM               031     //OPM
#define IRQ26_IDE               032     //HDD IDE INTERRUPT

// Pointer for an interrupt handler (should be a FAR routine)

typedef void (*IRQ_HANDLER)(void);

//
// Initialize the IRQ settings
//
extern void irq_init();

//
// Enable system-wide IRQ processing
//
extern void irq_enable_sys();

//
// Disable system-wide IRQ processing
//
extern void irq_disable_sys();

//
// Enable processing of an interrupt
//
// Inputs:
//      irq_num = the number of the interrupt
//
extern void irq_enable(short irq_num);

//
// Disable processing of an interrupt
//
// Inputs:
//      block = the number of the interrupt block (0 - 3)
//      irq_num = the number of the interrupt
//
extern void irq_disable(short irq_num);

//
// Clear a pending interrupt
//
// Inputs:
//      block = the number of the interrupt block (0 - 3)
//      irq_num = the number of the interrupt
//
extern void irq_clear(short irq_num);

//
// Register a FAR subroutine to be the handler for an interrupt.
//
// Inputs:
//      block = the number of the interrupt block (0 - 3)
//      irq_num = the number of the interrupt
//      handler = far pointer to the interrupt handler function
//
extern void irq_register(short irq_num, IRQ_HANDLER handler);

//
// Remove the registration of an IRQ handler
//
// Inputs:
//      block = the number of the interrupt block (0 - 3)
//      irq_num = the number of the interrupt
//
extern void irq_deregister(short irq_num);

#endif