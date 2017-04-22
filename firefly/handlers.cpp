
/*
 * Override default handlers for exceptions.
 *
 * HW fault handlers are weakly defined in gcc_startup.S
 * SW _exit() is defined by newlib-nano and can be redefined here (since newlib links last?)
 *
 * generic handlers are defined in lib nRF5x.
 * But you can't easily override the defaults from a library
 * unless you use the linker flag --whole-archive,
 * which is problematic using Eclipse.
 * So we override here but call the generic routines.
 */
#include <nRF5x.h>

extern "C" {

void HardFault_Handler();
void _exit();

void HardFault_Handler() {
	genericHardFaultHandler();
}

void _exit() {
	genericExitFaultHandler();
}

}
