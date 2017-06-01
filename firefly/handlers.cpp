
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
void NMI_Handler();
void SVC_Handler();
void PendSV_Handler();
void SysTick_Handler();
void _exit();

// Log distinguished flag
void HardFault_Handler() { genericHardFaultHandler(); }

// Log undistinguished flag
void NMI_Handler()    { genericExitHandler(); }
void SVC_Handler()    { genericExitHandler(); }
void PendSV_Handler() { genericExitHandler(); }
void SysTick_Handler(){ genericExitHandler(); }
void _exit()          { genericExitHandler(); }


void __assert_func(const char * fileName,
		int lineNumber,
		const char * functionName,
		const char * expression) {
	genericAssertionFaultHandler(fileName, lineNumber);
}


}
