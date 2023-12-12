#include <Arduino.h>
#include "drivers/lcd/lcd.h"

#define UART_SPEED 115200

void setup() {
	Serial.begin(UART_SPEED);
	Serial.print("hello there!\n");

	lcd_init();
}

void loop() {
	lcd_write("hello world!", 12);
	delay(5000);
}
