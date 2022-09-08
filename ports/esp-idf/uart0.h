#ifndef ESP_UART0_H
#define ESP_UART0_H

#include <stddef.h>
#include <stdint.h>

void uart0_init(uint32_t baudrate);
size_t uart0_write_async(void const *data, size_t datasize);
size_t uart0_read(void *buf, size_t bufsize);

#endif
