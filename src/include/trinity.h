/**
 * Definitions for the Trinity CPLD (joysticks, gamepads, DIP switches)
 */

// Dip switch Ports
#define DIP_USER            ((volatile byte *)0xAFE80D)     // Dip Switch 3/4/5 can be user Defined
#define DIP_BOOTMODE        ((volatile byte *)0xAFE80E)
#define BOOT_MODE0          0x01
#define BOOT_MODE1          0x02
#define DIP_BOOT_HDD        0x00
#define DIP_BOOT_SDCARD     0x01
#define DIP_BOOT_FLOPPY     0x02
#define DIP_BOOT_BASIC      0x03
#define HD_INSTALLED        0x80

// Boot modes
// DIP1  DIP2
// ----  ----
// OFF   OFF (1,1): Boot on BASIC
// ON    OFF (0,1): Boot from SD CARD
// OFF   ON  (1,0): Boot from Floppy (Illegal if MachineID doesn't specify a Floppy)
// ON    ON  (0,0): Boot from HDD (Illegal if Dip switch 8 is OFF (1))