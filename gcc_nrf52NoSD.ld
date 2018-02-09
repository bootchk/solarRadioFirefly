/* Linker script to configure memory regions. */

/* lkk this takes all available, i.e. no provision for Softdevice. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x80000
  RAM (rwx) :  ORIGIN = 0x20000000, LENGTH = 0x10000
}


INCLUDE "nrf52_common.ld"
