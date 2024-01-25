#ifndef __LCD_DRIVER_H__
#define __LCD_DRIVER_H__

#include <stddef.h>

void lcd_init(void);
size_t lcd_write(const char *buf, size_t size);

#endif /* __LCD_DRIVER_H__ */