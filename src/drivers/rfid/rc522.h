#ifndef __RFID_DRIVER_H__
#define __RFID_DRIVER_H__

#include <stddef.h>
#include <stdint.h>

#define UID_MAX_SIZE 4

void rfid_init(void);
bool rfid_available(void);
uint8_t rfid_read_tag(unsigned char *buf, size_t size);

#endif /* __RFID_DRIVER_H__ */