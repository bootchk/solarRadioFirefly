
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
 *
 * Other handlers ARE in nRF5x library, but always overwrite weak defs
 * since the app refers to symbols in the same object file as the strong handlers:
 * - POWER_CLOCK_IRQHandler, etc. other device handlers
 * - HardFault_Handler
 */

#include <radioSoC.h>

extern "C" {

// HardFault_Handler is overridden by library nRf5x

void NMI_Handler();
void SVC_Handler();
void PendSV_Handler();
void SysTick_Handler();

void _exit();


/*
 * Log an undistinguished flag
 *
 * Not expect NMI since we are not using its sources (external pin, configured NMI interrupts, MPU)
 *
 * Not using SVC or PendSV instructions anywhere.
 *
 * Not enabling SysTick.
 */
void NMI_Handler()    { genericExitHandler(); }
void SVC_Handler()    { genericExitHandler(); }
void PendSV_Handler() { genericExitHandler(); }
void SysTick_Handler(){ genericExitHandler(); }

void _exit()          { genericExitHandler(); }


/*
 * Replace builtin
 */
void __assert_func(const char * fileName,
		int lineNumber,
		const char * functionName,
		const char * expression) {
	genericAssertionFaultHandler(fileName, lineNumber);
}



/*
 * Handler for Nordic app_error library.
 * Replaces app_error_fault_handler in app_error_weak.c
 */
void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info) {
	// Dummy parameters
	genericAssertionFaultHandler("app_errr", pc);
}


}	// extern C

