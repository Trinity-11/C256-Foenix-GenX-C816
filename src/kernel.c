/**
 * The new portable C kernel for the C256.
 */

#include <string.h>
#include <sys/types.h>

#include "constants.h"
#include "debug.h"
#include "interrupt.h"
#include "dev/clock.h"
#include "dev/codec.h"
#include "dev/psg.h"
#include "dev/sid.h"
#include "dev/channel/channel.h"
#include "dev/channel/text_screen.h"
#include "dev/channel/ps2.h"
#include "dev/block/block.h"
#include "dev/block/sdc.h"
#include "dev/block/pata.h"
#include "fatfs/ff.h"
#include "cli/cli.h"

//
// Declare the heap
//

void *heap_start = (void * )MIN_HEAP;
void *heap_end = (void * )MAX_HEAP;

void k_prints(char * s) {
    chan_write(CDEV_CONSOLE, s, strlen(s));
}

void ltoa(long x, char * text) {
    char buffer[40];
    short digit;
    long running;
    short i;
    short is_negative = 0;

    if (x < 0) {
        is_negative = 1;
        running = -x;
    } else {
        running = x;
    }

    buffer[39] = 0;
    for (i = 38; i > 0; ) {
        digit = running % 10;
        running = running / 10l;
        buffer[i--] = '0' + digit;

        if (running == 0) break;
    }

    if (is_negative) {
        buffer[i--] = '-';
    }

    do {
        *text = buffer[++i];
        text++;
    } while (i < 40);
}

//
// Initialize all the hardware on the Foenix
//
void fnx_init() {
    short res;

    irq_init();
    txt_scr_init();

    TRACE("fnx_init");

    clk_init();
    codec_init();
    psg_init();
    sid_init();
    bdev_init_system();

    res = ps2_init();
    if (res != 0) {
        k_prints("Could not initialize keyboard: ");
        txt_prhex_w(0, res);
        irq_disable(IRQ08_KBD);

        while (1) ;
    }

    res = con_install();
    if (res != 0) {
        k_prints("Could not install the console driver: ");
        txt_prhex_w(0, res);

        while (1) ;
    }

    res = sdc_install();
    if (res != 0) {
        k_prints("Could not install the SDC driver: ");
        txt_prhex_w(0, res);

        while (1) ;
    }

    res = pata_install();
    if (res != 0) {
        k_prints("Could not install the HDD driver: ");
        txt_prhex_w(0, res);

        while (1) ;
    }

    res = cli_init();
    if (res != 0) {
        k_prints("Could not initilize the command line interface: ");
        txt_prhex_w(0, res);

        while (1) ;
    }   
}

void display_pata() {
    char buffer[20];
    t_drive_info di;
    short res;

    res = bdev_ioctrl(BDEV_HDC, PATA_GET_DRIVE_INFO, (byte *)&di, sizeof(t_drive_info));
    if (res) {
        k_prints("Could not get HDD information.\r");
    } else {
        k_prints("Installed PATA Hard Drive:\rModel: ");
        chan_write(0, di.model_name, sizeof(di.model_name));
        
        k_prints("\rVersion: ");
        chan_write(0, di.firmware_version, sizeof(di.firmware_version));
        
        k_prints("\rSerial Number: ");
        chan_write(0, di.serial_number, sizeof(di.serial_number));

        k_prints("\rDefault LBA: ");
        // ltoa(di.default_lba, buffer);
        // chan_write(0, buffer, sizeof(buffer));
        txt_prhex_w(0, ((di.l.lba_default >> 16) & 0xffff));
        txt_prhex_w(0, (di.l.lba_default) & 0xffff);

        k_prints("\r\r");
    }
}

int main(int argc, char * argv[]) {
    short res;
    FRESULT f_result;

    fnx_init();

    k_prints("Foenix GenX Portable Kernel v00.00.00\r");

    display_pata();

    k_prints("OK\r");
    
    while (1) {
        cli_repl();
    }
}