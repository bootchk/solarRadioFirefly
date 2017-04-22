
/*
 * Override default handlers, weakly defined in gcc_startup.S
 *
 * Note also that nRF5x redefines nanolib _exit()
 */
#include <nRF5x.h>

extern "C"

void HardFault_Handler() {
	genericHardFaultHandler();
}
