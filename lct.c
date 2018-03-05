#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <time.h>
#include "lcd.h"
#include "getter.h"
#include "chGenerator.h"

#define CHAR_WIDTH 5
#define LCD_CHAR_WIDTH 10

int main(void) {

    struct lcd l;
    

    uint8_t customChar[8] = {
        0b00000,
        0b00000,
        0b01010,
        0b00000,
        0b10001,
        0b01110,
        0b00000,
        0b00000
    };

    uint8_t customChar1[8] = {
        0b11111,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b11111
    };

    uint8_t customChar2[8] = {
        0b10001,
        0b00000,
        0b00100,
        0b01010,
        0b01110,
        0b01010,
        0b00000,
        0b10001
    };

    uint8_t screen1[8] = {
        0b11111,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b11111
    };

    uint8_t screen2[8] = {
        0b00000,
        0b01110,
        0b01010,
        0b01010,
        0b01010,
        0b01010,
        0b01110,
        0b00000
    };

     uint8_t screen3[8] = {
        0b00000,
        0b00000,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00000,
        0b00000
    };

    uint8_t empty[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000
    };

    uint8_t t[8] = {
        0b00000,
        0b11111,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00000
    };

    uint8_t invt[8] = {
        0b11111,
        0b00000,
        0b11011,
        0b11011,
        0b11011,
        0b11011,
        0b11011,
        0b11111
    };
    


    addCustomChars(customChar, "smiley");
    addCustomChars(customChar1, "cursor");
    addCustomChars(customChar2, "A");
    addCustomChars(screen1, "sc1");
    addCustomChars(screen2, "sc2");
    addCustomChars(screen3, "sc3");
    addCustomChars(empty, "empty");
    addCustomChars(t, "t");
    addCustomChars(invt, "invt");

    // Address of I2C is 0x3f. Need to initialize two times, otherwise 
    // LCD displays garbage
    if (lcd_setup(&l, "/dev/i2c-0", 0x3f, 16, 2) < 0) {  
        perror("lcd_setup");
        exit(EXIT_FAILURE);
    }
    if (lcd_setup(&l, "/dev/i2c-0", 0x3f, 16, 2) < 0) {  
        perror("lcd_setup");
        exit(EXIT_FAILURE);
    }
    // End of init

    lcd_clear(&l);

    char* line1 = "lcd.lcd.show_lines1";
    char* line2 = "lcd.lcd.show_lines2";

    char* buffer = (char *) malloc(16);
    

    getText(line1, buffer);

    switch()
    {
        

    }
    lcd_print(&l, buffer);



    /*lcd_cursor_pos(&l, 1, 0);
    getText(buf1, buffer);
    lcd_print(&l, buffer);*/

   /* while(1){
        sleep(2);
        
        l.animationX = 14;
        l.animationY = 0;
        lcd_cgram_char(&l, getCustomCharById("t"));

        sleep(2);
        
        l.animationX = 14;
        l.animationY = 0;
        lcd_cgram_char(&l, getCustomCharById("invt"));

        //sleep(2);
        
        //l.animationX = 14;
        //l.animationY = 0;
        //lcd_cgram_char(&l, getCustomCharById("empty"));
    }*/
    
    return 0;
};
