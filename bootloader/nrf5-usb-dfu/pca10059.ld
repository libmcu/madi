/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx)	: ORIGIN = 0x00027000, LENGTH = 0x3000
  RAM (rwx)	: ORIGIN = 0x200039d8, LENGTH = 0x3c228
}


INCLUDE "nrf_common.ld"
