#ifndef STM32F1_UART1_H
#define STM32F1_UART1_H

#include <stddef.h>
#include <stdint.h>

void uart1_init(uint32_t baudrate);
size_t uart1_write(void const *data, size_t datasize);
size_t uart1_read(void *buf, size_t bufsize);

#endif
