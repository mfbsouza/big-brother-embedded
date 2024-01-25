#include "lcd.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <stdint.h>
#include <stddef.h>

#define ADDR 0x27
#define WIDTH 16
#define HEIGHT 2

LiquidCrystal_I2C lcd(ADDR, WIDTH, HEIGHT);

void lcd_init(void)
{
    lcd.init();
    lcd.backlight();
}

size_t lcd_write(const char *buf, size_t size)
{
    size_t to_write, lines;
    uint8_t offset = 0;
    uint8_t cur_line = 0;
    char tmp[WIDTH*HEIGHT];
    String str;

    to_write = (size <= (WIDTH*HEIGHT)) ? size : (WIDTH*HEIGHT);
    lines = (to_write/WIDTH) + 1;

    memset(tmp, 0, WIDTH*HEIGHT);
    memcpy(tmp, buf, to_write);
    str = String(tmp);

    lcd.clear();
    while (lines > 0) {
        if (lines == 1) {
            lcd.setCursor(0, cur_line);
            lcd.print(str.substring(offset, (offset + to_write)));
            //lcd.print("TESTE!");
            to_write -= to_write;
        } else {
            lcd.setCursor(0, cur_line);
            lcd.print(str.substring(offset, ((cur_line + 1) * WIDTH)));
            to_write -= WIDTH;
            offset += WIDTH;
            cur_line += 1;
        }
        lines -= 1;
    }
    
    return to_write;
}