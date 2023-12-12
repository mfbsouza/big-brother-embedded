#ifndef __IO_INTERFACE_H__
#define __IO_INTERFACE_H__

#include <stddef.h>
#include <stdint.h>

struct Io {
	size_t (*available) (void);
	size_t (*read) (uint8_t *buf, size_t size);
	size_t (*write) (uint8_t *buf, size_t size);
};

#endif /* __IO_INTERFACE_H__ */