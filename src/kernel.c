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

    res = cli_init();
    if (res != 0) {
        k_prints("Could not initilize the command line interface: ");
        txt_prhex_w(0, res);

        while (1) ;
    }   
}

int main(int argc, char * argv[]) {
    short res;
    FRESULT f_result;

    fnx_init();

    k_prints("Foenix GenX Portable Kernel v00.00.00\r");

    k_prints("OK\r");
    
    while (1) {
        cli_repl();
    }
}