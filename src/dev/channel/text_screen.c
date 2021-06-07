/**
 * Code to drive the text mode screens (main and EVID)
 */

#include "vicky_general.h"
#include "types/types.h"
//#include "rsrc/font/MSX_8x8.h"
#include "rsrc/font/foenix_st_8_8.h"

#define MAX_TEXT_SCREENS 2

//
// Structure to describe the text screens.
//
// This structure captures the pointers to the various hardware registers and memory
// locations to control a particular text screen as well as tracking the layout of the
// screen and the cursor location.
//

struct s_screen {
    volatile char * text_begin;                 // Pointer to the beginning of the text matrix
    volatile char * color_begin;                // Pointer to the beginning of the color matrix
    volatile char * text_position;              // Pointer to the current position in the text matrix
    volatile char * color_position;             // Pointer to the current position in the color matrix
    volatile short *hw_cursor_x;                // Pointer to the screen's hardware cursor column coordinate
    volatile short *hw_cursor_y;                // Pointer to the screen's hardware cursor row coordinate
    volatile char *hw_resolution;               // Pointer to the screen's hardware text resolution register
    volatile char *hw_border_control;           // Pointer to the screen's hardware border control register
    volatile t_color3 *hw_border_color;         // Pointer to the border color
    volatile char *hw_border_size_x;            // Pointer to the screen's hardware border width register
    volatile char *hw_border_size_y;            // Pointer to the screen's hardware border height register
    volatile t_color4 *hw_foreground_colors;    // Pointer to the screen's foreground LUT
    volatile t_color4 *hw_background_colors;    // Pointer to the screen's background LUT
    volatile char *hw_font;                     // Pointer to the font area
    volatile t_color3 *hw_background_color;     // Pointer to the screen's background color
    volatile byte * hw_cursor_control;          // Pointer to the screen's cursor control register
    volatile char * hw_cursor_character;        // Pointer to the screen's cursor character register

    int column;
    int columns_max;
    int columns_visible;
    int row;
    int rows_max;
    int rows_visible;
    char color;
};

//
// Global variable to hold the screen variables
//

short g_num_screens = 0;
struct s_screen g_screens[MAX_TEXT_SCREENS];

//
// Initial color LUT data
//

byte g_txt_init_colors[16][3] = {
    { 0, 0, 0},
    { 128, 0, 0},
    { 0, 128, 0},
    { 128, 128, 0},
    { 0, 0, 128},
    { 128, 0, 128},
    { 0, 128, 128},
    { 192, 192, 192},
    { 128, 128, 128},
    { 255, 0, 0},
    { 0, 255, 0},
    { 255, 255, 0},
    { 0, 0, 255},
    { 255, 0, 255},
    { 0, 255, 255},
    { 255, 255, 255}
};

/**
 * Scroll the screen up by one line.
 */
void txt_scroll_up(int screen) {
    int row;
    int column;

    if (screen < g_num_screens) {
        struct s_screen *s = &g_screens[screen];

        // Copy each character and its color up one line

        for (row = 0; row < s->rows_visible - 1; row++) {
            for (column = 0; column < s->columns_visible; column++) {
                int base = row * s->columns_max + column;
                s->text_begin[base] = s->text_begin[base + s->columns_max];
                s->color_begin[base] = s->color_begin[base + s->columns_max];
            }
        }

        // Set the bottom line to blank, and set the color to the default

        row = s->rows_visible - 1;
        for (column = 0; column < s->columns_visible; column++) {
            int base = row * s->columns_max + column;
            s->text_begin[base] = ' ';
            s->color_begin[base] = s->color;
        }
    }
}

/**
 * Make sure the cursor position is valid.
 * Position the hardware cursor at the correct position.
 * Recompute text and color pointers, if needed.
 */
void txt_locate(int screen, short x, short y) {
    long offset;

    if (screen < g_num_screens) {
        struct s_screen *s = &g_screens[screen];

        s->column = x;
        s->row = y;

        if (s->column >= s->columns_visible) {
            s->column = 0;
            s->row++;
        }   

        if (s->row >= s->rows_visible) {
            s->row = s->rows_visible - 1;
            txt_scroll_up(screen);
        }

        *s->hw_cursor_x = s->column;
        *s->hw_cursor_y = s->row;

        offset = s->columns_max * s->row + s->column;
        s->text_position = s->text_begin + offset;
        s->color_position = s->color_begin + offset;
    }
}

/**
 * Compute the geometry of the text screen based on its resolution and border settings
 */
void txt_setsize(int screen) {
    if (screen <g_num_screens) {
        struct s_screen *s = &g_screens[screen];

        short resolution = (short)*s->hw_resolution;
        short border = (short)(*s->hw_border_control & Border_Ctrl_Enable);
        short border_x = (short)(*s->hw_border_size_x);
        short border_y = (short)(*s->hw_border_size_y);

        switch (resolution) {
            case VKY_RESOLUTION_320_240:
                s->columns_max = 40;
                s->rows_max = 30;
                break;

            case VKY_RESOLUTION_800_600:
                s->columns_max = 100;
                s->rows_max = 75;
                break;

            case VKY_RESOLUTION_400_300:
                s->columns_max = 50;
                s->rows_max = 34;
                break;

            default:
            case VKY_RESOLUTION_640_480:
                s->columns_max = 80;
                s->rows_max = 60;
                break;
        }

        if (border) {
            s->columns_visible = s->columns_max - (border_x / 4);
            s->rows_visible = s->rows_max - (border_y / 4);
        } else {
            s->columns_visible = s->columns_max;
            s->rows_visible = s->rows_max;
        }
    }
}

/**
 * Clear the text screen
 */
void txt_scr_clear(int screen) {
    int i;

    if (screen < g_num_screens) {
        struct s_screen *s = &g_screens[screen];
        for (i = 0; i < s->columns_max * s->rows_max; i++) {
            s->color_begin[i] = s->color;
            s->text_begin[i] = ' ';
        }
    }
}

/**
 * Initilialize the text screens
 */
void txt_scr_init() {
    volatile byte *vicky_master_control = (byte *)VKY_MASTER_CONTROL_L;
    int x;
    int color;
    byte color_component[3];

    struct s_screen *s = &g_screens[0];

    g_num_screens = 1;  // For now, there's just the one screen

    // Set up the memory locations for this screen

    s->text_begin = (char *)CS_TEXT_MEM_PTR;
    s->color_begin = (char *)CS_COLOR_MEM_PTR;
    s->hw_resolution = (char *)VKY_MASTER_CONTROL_H;
    s->hw_border_control = (char *)BORDER_CTRL_REG;
    s->hw_border_color = (t_color3 *)BORDER_COLOR_B;
    s->hw_border_size_x = (char *)BORDER_X_SIZE;
    s->hw_border_size_y = (char *)BORDER_Y_SIZE;
    s->hw_cursor_x = (short *)VKY_TXT_CURSOR_X_REG_L;
    s->hw_cursor_y = (short *)VKY_TXT_CURSOR_Y_REG_L;
    s->hw_foreground_colors = (t_color4 *)FG_CHAR_LUT_PTR;
    s->hw_background_colors = (t_color4 *)BG_CHAR_LUT_PTR;
    s->hw_font = (char *)FONT_MEMORY_BANK0;
    s->hw_background_color = (t_color3 *)BACKGROUND_COLOR_B;
    s->hw_cursor_control = (byte *)VKY_TXT_CURSOR_CTRL_REG;
    s->hw_cursor_character = (char *)VKY_TXT_CURSOR_CHAR_REG;

    // Set the screen to text mode
    // TODO: check gamma DIP switch
    
    *vicky_master_control = Mstr_Ctrl_Text_Mode_En;

    // Set the screen's resolution

    // TODO: consult the high res DIP switch

    if (*s->hw_resolution & 0x01) {
        // If we were in 800x600 or 400x300, we need to do this little dance
        // to make sure the PLL properly synchs to the right clock rate for 640x480

        *s->hw_resolution = VKY_RESOLUTION_640_480; // Switch to 640x480 mode temporarily
        for (x = 0; x < 5; x++) ;                   // Wait for the PLL to lock on
        *s->hw_resolution = VKY_RESOLUTION_800_600; // Switch to 800x600 temporarily
        for (x = 0; x < 5; x++) ;                   // Wait for the PLL to lock on
    }

    *s->hw_resolution = VKY_RESOLUTION_640_480;     // Switch to 640x480 mode

    // Set up the border

    *s->hw_border_control = Border_Ctrl_Enable;
    *s->hw_border_size_x = 32;
    *s->hw_border_size_y = 32;
    s->hw_border_color->red = 0x00;
    s->hw_border_color->green = 0x00;
    s->hw_border_color->blue = 0x80;

    // Set the sizes on the screen

    txt_setsize(0);

    // Set the text color LUTs

    for (color = 0; color < MAX_TEXT_COLORS; color++) {
        volatile t_color4 *lut_entry;

        lut_entry = &s->hw_foreground_colors[color];
        lut_entry->red = g_txt_init_colors[color][0];
        lut_entry->green = g_txt_init_colors[color][1];
        lut_entry->blue = g_txt_init_colors[color][2];
        lut_entry->alpha = 0xff;

        lut_entry = &s->hw_background_colors[color];
        lut_entry->red = g_txt_init_colors[color][0];
        lut_entry->green = g_txt_init_colors[color][1];
        lut_entry->blue = g_txt_init_colors[color][2];
        lut_entry->alpha = 0xff;
    }

    s->hw_background_color->red = 0x00;
    s->hw_background_color->blue = 0x80;
    s->hw_background_color->green = 0x00;

    // Set the default color and clear the screen
    
    s->color = 0xFC;
    txt_scr_clear(0);

    // Move to the upper left corner

    txt_locate(0, 0, 0);

    // Copy the font into the font storage area...

    for (x = 0; x < 256*8; x++) {
        s->hw_font[x] = foenix_st_8x8[x];
    }

    // Set up the cursor
    *s->hw_cursor_control = Vky_Cursor_Enable;      // Enable the cursor
    *s->hw_cursor_character = (char)0xdb;           // Set the character used for the cursor

    // TODO: determine if the EVID screen is present and initialize it
}

/**
 * Print a character c to the current cursor position on the indicated screen.
 *
 * Inputs:
 * c = character to print
 * screen = the number of the screen (0 = main, 1 = EVID)
 */
void txt_raw_putc(int screen, char c) {
    if (screen <g_num_screens) {
        struct s_screen *s = &g_screens[screen];

        switch (c) {
            case '\r':
                // Handle carriage return
                txt_locate(screen, 0, s->row+1);
                break;

            case '\t':
                txt_locate(screen, (s->column & 0xfff8) + 8, s->row);
                break;

            case '\b':
                if (s->column > 0) {
                    txt_locate(screen, s->column - 1, s->row);
                    *s->text_position = ' ';
                    *s->color_position = s->color;
                }
                break;

            default:
                *s->text_position++ = c;
                *s->color_position++ = s->color;
                txt_locate(screen, s->column + 1, s->row);
        }

    }
}

/**
 * Print a character c to the current cursor position on the indicated screen.
 * 
 * Interpret escape codes for various functions
 *
 * Inputs:
 * c = character to print
 * screen = the number of the screen (0 = main, 1 = EVID)
 */
void txt_putc(int screen, char c) {
    // TODO: implement the ANSI escapes...
    
    txt_raw_putc(screen, c);
}

// Print a byte in hex
void txt_prhex_b(int screen, byte data) {
    const char hex_codes[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    short msn = (data >> 4) & 0x0f;
    short lsn = (data & 0x0f);

    txt_raw_putc(screen, hex_codes[msn]);
    txt_raw_putc(screen, hex_codes[lsn]);
}

// Print a word in hex
void txt_prhex_w(int screen, short data) {
    txt_prhex_b(screen, (data & 0xff00) >> 8);
    txt_prhex_b(screen, data & 0x00ff);
}
