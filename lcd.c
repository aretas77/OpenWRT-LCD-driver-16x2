#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "lcd.h"
#include "chGenerator.h"
#include "includes.h"

/* I2C expander and LCD wiring (HD44780)
    LCD     I2C expander    meaning
    -------------------------------
    RS      P(0)            command = 0, data = 1
    RW      P(1)            read = 1, write = 0 - we don't use this
    EN      P(2)            enable lines, if set to 1 LCD reads input
    BL      P(3)            backlight, on = 1, off = 1
    D4      P(4)            data, 4-bit interface
    D5      P(5)
    D6      P(6)
    D7      P(7)

    Data to LCD is sent using 4-bits. 
    First goes upper 4 bits (D4-D7) then lower 4 bits (D3-D0)
    Datasheet used: https://www.sparkfun.com/datasheets/LCD/GDM1602K-Extended.pdf
*/

#define PCF8574_RS 0b00000001
#define PCF8574_RW 0b00000010
#define PCF8574_EN 0b00000100
#define PCF8574_BL 0b00001000


#define CGRAM_1 0x10
#define CGRAM_2 0x08
#define CGRAM_3 0x04
#define CGRAM_4 0x02
#define CGRAM_5 0x01

#define CMD_CLEAR 0x01
#define CMD_ENTRY 0x06
#define CMD_OFF   0x08
#define CMD_ON    0x0D
#define CMD_ON_NO_CURSOR    0x0C
#define CMD_ON_BLINK_CURSOR 0x0F
#define CMD_ON_SOLID_CURSOR 0x0E

#define CMD_SHIFT_LEFT  0x18
#define CMD_SHIFT_RIGHT 0x1C

#define CMD_FIRST_LINE  0x80  // 0x80 to 0x90
#define CMD_SECOND_LINE 0xC0  // 0xC0 to 0xD0 (2-line mode)
#define CMD_THIRD_LINE  0x00  // undefined
#define CMD_FOURTH_LINE 0x00  // unedfined

#define CMD_CGRAM_OFFSET 0x40
#define CMD_DDRAM_OFFSET 0x80

enum LCD_Input
{
    DATA,
    COMMAND
};

static uint8_t line_offsets[] = {CMD_FIRST_LINE, CMD_SECOND_LINE};

static void lcd_init(struct lcd *l);
static void lcd_write(struct lcd *l, uint8_t data, enum LCD_Input type);
static void i2c_write(struct lcd *l, uint8_t byte);

int lcd_setup(struct lcd *l, const char *device, uint8_t i2c_addr, int c, int r)
{
    usleep(50000);
    l->rows = r;
    l->cols = c;
    l->buf_bsize = sizeof(uint8_t) * r * c;
    l->buffer = (uint8_t *)malloc(l->buf_bsize);
    memset(l->buffer, ' ', l->buf_bsize);
    l->buf_pos = 0;
    l->direct = 1;
    l->on = 1;
    l->animationX = -1;
    l->animationY = -1;
    l->customChars = 0;
    l->cursor = CMD_ON_NO_CURSOR;
    l->x = 0;
    l->y = 0;

    l->fd = open(device, O_RDWR);
    l->backlight = PCF8574_BL;
    if (l->fd < 0)
    {
        return -errno;
    }
    if (ioctl(l->fd, I2C_SLAVE, i2c_addr) < 0)
    {
        return -errno;
    }

    /* 
        test read from expander - just to check
        early if device is accessible 
    */
    uint8_t expander_register;
    if (read(l->fd, &expander_register, 1) < 0)
    {
        return -errno;
    }
    lcd_init(l);
    return 0;
}

void lcd_cursor_pos(struct lcd *l, int r, int c)
{
    r = r % l->rows;
    c = c % l->cols;

    l->x = c;
    l->y = r;

    lcd_write(l, line_offsets[r] + c, COMMAND);
    return;
}

void lcd_shift_right(struct lcd *l)
{
    lcd_write(l, CMD_SHIFT_RIGHT, COMMAND);
}

void lcd_shift_left(struct lcd *l)
{
    lcd_write(l, CMD_SHIFT_LEFT, COMMAND);
}

void lcd_off(struct lcd *l)
{
    l->on = 0;
    lcd_write(l, CMD_OFF, COMMAND);
}

void lcd_on(struct lcd *l)
{
    l->on = 1;
    lcd_write(l, l->cursor, COMMAND);
}

void lcd_cursor_no(struct lcd *l)
{
    l->cursor = CMD_ON_NO_CURSOR;
    if (l->on)
        lcd_on(l);
}

void lcd_cursor_solid(struct lcd *l)
{
    l->cursor = CMD_ON_SOLID_CURSOR;
    if (l->on)
        lcd_on(l);
}

void lcd_cursor_blink(struct lcd *l)
{
    l->cursor = CMD_ON_BLINK_CURSOR;
    if (l->on)
        lcd_on(l);
}

void lcd_backlight_on(struct lcd *l)
{
    l->backlight = PCF8574_BL;
    i2c_write(l, l->backlight);
    return;
}

void lcd_backlight_off(struct lcd *l)
{
    l->backlight = 0;
    i2c_write(l, l->backlight);
    return;
}

void lcd_clear(struct lcd *l)
{
    // Turn off display
    lcd_write(l, 0x08, COMMAND);

    // Clear display 
    lcd_write(l, CMD_CLEAR, COMMAND);
    usleep(200);

    // Display on
    lcd_write(l, 0x0F, COMMAND);

    // Clear 
    l->x = 0;
    l->y = 0;
}

void lcd_print(struct lcd *l, const char *str)
{   
    // Create temporary string from the given string
    char* tmp = (char *) malloc(strlen(str));
    strcpy(tmp, str);

    char* ch = checkForCode(tmp);
    //printf("\n%s\n", ch);
    unsigned i;
    for (i = 0; i < strlen(tmp); i++)
    {   
        // '`' specifies the place where the custom char should be inserted
        if(tmp[i] == '`' && ch != NULL) 
        {
            
            uint8_t* cgram = getCustomCharById(ch);
            if(cgram != NULL)
            {
                lcd_cgram_char(l, cgram);
                l->customChars = 8;
            }
            
        }
        else
        {
            lcd_write(l, tmp[i], DATA);
        }
    }
    free(tmp);
}

void lcd_buffer_flush(struct lcd *l)
{
    printf("flush\n");
    int i, j;
    //lcd_cursor_pos(l, 0, 0);
    int row_order[] = {0, 1};
    for (i = 0; i < l->rows; i++)
    {
        for (j = 0; j < l->cols; j++)
        {
            l->direct = 1;
            lcd_write(l, l->buffer[row_order[i] * l->cols + j], DATA);
            l->direct = 0;
        }
    }
    printf("flush done\n\n");
}

void lcd_buffer_on(struct lcd *l)
{
    l->direct = 0;
}

void lcd_buffer_off(struct lcd *l)
{
    l->direct = 1;
}

void lcd_cgram_define(struct lcd *l, int position, uint8_t rows[8]) 
{
    int x = l->x;
    int y = l->y;
    // Set the CGRAM address
    lcd_write(l, CMD_CGRAM_OFFSET + position, COMMAND);

    int i;
    for (i = 0; i < 8; i++) 
    {
        lcd_write(l, rows[i], DATA);
    }
    if(l->animationX != -1 && l->animationY != -1)
    {
        lcd_cursor_pos(l, l->animationY, l->animationX);
        l->animationX = -1;
        l->animationY = -1;
    }
    else
    {
        lcd_cursor_pos(l, y, x);
    }
    if(l->customChars > 0)
    {
        lcd_write(l, 1, DATA);
    }
    else
    {
        lcd_write(l, 0, DATA);
    }
    
}

void lcd_cgram_char(struct lcd *l, uint8_t code[8])
{
    lcd_cgram_define(l, 0 + l->customChars, code);  
}

/*
    Problem: When used in 1 line mode, text displays well,
    as for 2-line mode, text becomes dimmer.

    Speculation: Could be that LCD needs more power to operate
    in 2-line mode, so text becomes more dim.

    Similar problem:
    http://www.edaboard.com/showthread.php?t=264732
*/
static void lcd_init(struct lcd *l)
{
    usleep(15000);

    /* initialize display to use 4-bit interface */
    i2c_write(l, 0b0011);
    i2c_write(l, 0b0011 | PCF8574_EN);
    usleep(5000);

    i2c_write(l, 0b0011);
    i2c_write(l, 0b0011 | PCF8574_EN);
    usleep(200);

    i2c_write(l, 0b0011);
    i2c_write(l, 0b0011 | PCF8574_EN);
    usleep(200);

    /* Initialize display in 4-bit mode */
    i2c_write(l, 0b0010);
    i2c_write(l, 0b0010 | PCF8574_EN);
    usleep(200);
    
    i2c_write(l, 0b0010);
    i2c_write(l, 0b0010 | PCF8574_EN);
    usleep(500);

    // Display on, cursor on
    lcd_write(l, 0x28, COMMAND); 
   
    // Entry mode on
    lcd_write(l, 0x06, COMMAND);

    // Turn off display
    lcd_write(l, 0x08, COMMAND);

    /* clear display */
    lcd_write(l, 0x01, COMMAND);

    // Cursor to the right
    lcd_write(l, 0x02, COMMAND);

    // Display on
    lcd_write(l, 0x0F, COMMAND);

    return;
}

static void lcd_write(struct lcd *l, uint8_t data, enum LCD_Input type)
{
    uint8_t low_bits = (data & 0x0f) << 4;
    uint8_t high_bits = data & 0xf0;

    uint8_t control = l->backlight;
    int delay = 0;
    switch (type)
    {
    case DATA:
        control = control | PCF8574_RS;

        // Shift cursor to the right
        l->x++; 
        delay = 300; /* wait 300us for data writes */
        break;
    case COMMAND:
        delay = 5000; /* wait 5ms for command execution */
        break;
    default:
        fprintf(stderr, "unknown data type");
    }

    if (l->direct)
    {

        /* write higher part of the byte */
        i2c_write(l, high_bits | control);
        usleep(200);
        i2c_write(l, high_bits | control | PCF8574_EN);
        usleep(200);
        i2c_write(l, high_bits | control);
        usleep(delay);
        /* write lower part of the byte */
        i2c_write(l, low_bits | control);
        usleep(200);
        i2c_write(l, low_bits | control | PCF8574_EN);
        usleep(200);
        i2c_write(l, low_bits | control);
        usleep(delay);
    }
    else
    {
        l->buffer[l->buf_pos++] = data;
    }
    return;
}

static void i2c_write(struct lcd *l, uint8_t byte)
{
    int ret = write(l->fd, &byte, 1);
    if (ret < 1)
    {
        perror("i2c_write");
    }
}
