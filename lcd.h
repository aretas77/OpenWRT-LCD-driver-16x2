#include <inttypes.h>

/*
    Library for interacting with LCD. 
    Always call lcd_setup first.
*/

#ifndef LCD_H
#define LCD_H



// LCD struct 
struct lcd {
    int fd;                 // File descriptor
    int cols;               // LCD column count
    int rows;               // LCD row count
    int on;                 // status of LCD
    uint8_t backlight;      // status of LCD backlight
    uint8_t cursor;         // cursor settings 
    uint8_t *buffer;        // buffer for text
    int x;                  // x position of cursor (column)
    int y;                  // y position of cursor (row)
    int animationX;
    int animationY;
    int customChars;        // how many custom chars exist in CGRAM
    int buf_pos;
    int buf_bsize;
    int direct;
};

//! initialize new lcd struct
/*! \param l pointer to the lcd struct
    \param path is path to the i2c device (/dev/i2c-0)
    \param addr is i2c address of the LCD
    \param r number of rows
    \param c number of columns */
int     lcd_setup(struct lcd* l, const char* path, uint8_t addr, int r, int c);

//! move cursor to the specified position
/*! \param r destination row
    \param c destination column
    \param l pointer to the lcd struct */
void    lcd_cursor_pos(struct lcd *l, int r, int c);

//! shift LCD memory one character to the right
/*! \param l pointer to the lcd struct */
void    lcd_shift_right(struct lcd *l);

//! shift LCD memory one character to the left
/*! \param l pointer to the lcd struct */
void    lcd_shift_left(struct lcd *l);

//! turn off the LCD
/*! \param l pointer to the lcd struct */
void    lcd_off(struct lcd *l);

//! turn on the LCD
/*! \param l pointer to the lcd struct */
void    lcd_on(struct lcd *l);

//! do not show cursor
/*! \param l pointer to the lcd struct */
void    lcd_cursor_no(struct lcd *l);

//! show solid cursor
/*! \param l pointer to the lcd struct */
void    lcd_cursor_solid(struct lcd *l);

//! show blinking cursor
/*! \param l pointer to the lcd struct */
void    lcd_cursor_blink(struct lcd *l);

//! turn on the lcd backlight
/*! \param l pointer to the lcd struct */
void    lcd_backlight_on(struct lcd *l);

//! turn off the lcd backlight
/*! \param l pointer to the lcd struct */
void    lcd_backlight_off(struct lcd *l);

//! clear LCD memory (screen)
/*! \param l pointer to the lcd struct */
void    lcd_clear(struct lcd *l);

//! print string at current position
/*! \param str string to display
    \param l pointer to the lcd struct */
void    lcd_print(struct lcd *l, const char *str);


void    lcd_cgram_char(struct lcd *l, uint8_t code[8]);

//! define custom character pattern
/*! \param position index in pattern (CGRAM) memory
    \param rows byte array of matrix (each byte represents one row)
    \param l pointer to the lcd struct */
void    lcd_cgram_define(struct lcd *l, int position, uint8_t rows[8]);

// Functions for printing string first to buffer then to LCD
// For LCD speed 
void    lcd_buffer_flush(struct lcd *l);
void    lcd_buffer_on(struct lcd *l);
void    lcd_buffer_off(struct lcd *l);

#endif
