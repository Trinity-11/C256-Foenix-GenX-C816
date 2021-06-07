/**
 * Implementation of DOS commands
 */

#include <stdlib.h>
#include "debug.h"
#include "cli/cli.h"
#include "cli/dos_commands.h"
#include "dev/channel/channel.h"
#include "fatfs/ff.h"

//
// Global variables for the file commands
//

DIR my_dir;
FILINFO my_file;
FATFS my_fs;
char line[255];

//
// Implementation of a simple TYPE command
//
short dos_cmd_type(char * parameters) {
    FRESULT fres;

    TRACE("dos_cmd_type");

    fres = f_mount(&my_fs, "0:", 0);
    if (fres == FR_OK) {
        fres = f_open(&my_file, parameters, FA_READ);
        if (fres == FR_OK) {
            while (f_gets(line, sizeof line, &my_file)) {
                chan_write(0, line, sizeof(line));
                chan_write_b(0, '\r');
            }
            f_close(&my_file);

        } else {
            char * err = "Could not open file.\r";
            chan_write(0, err, strlen(err));
            return -1;
        }
    }

    f_mount(0, "", 1);
}

//
// Implementation of a simple DIR command
//
short dos_cmd_dir(char  * parameters) {
    FRESULT fres;

    TRACE("dos_cmd_dir");

    fres = f_mount(&my_fs, "0:", 0);
    TRACE("f_mount");
    if (fres == FR_OK) {
        fres = f_opendir(&my_dir, "/");
        TRACE("f_opendir");
        if (fres == FR_OK) {
            do {
                fres = f_readdir(&my_dir, &my_file);
                TRACE("f_readdir");
                if ((fres == FR_OK) && (my_file.fname[0] != 0)) {
                    if ((my_file.fattrib & AM_HID) == 0) {
                        chan_write(0, my_file.fname, strlen(my_file.fname));
                        if (my_file.fattrib & AM_DIR) {
                            chan_write_b(0, '/');
                        }
                        chan_write_b(0, '\r');
                    }
                } else {
                    break;
                }
            } while(1);

            f_closedir(&my_dir);
        } else {
            char * err = "Could not open directory.\r";
            chan_write(0, err, strlen(err));
        }

        f_mount(0, "", 0);
    } else {
        char * err = "Could not mount drive.\r";
        chan_write(0, err, strlen(err));        
    }

    return 0;
}

//
// Register the DOS commands
//
short cli_dos_register() {
    cli_register("DIR", dos_cmd_dir);
    cli_register("TYPE", dos_cmd_type);
}