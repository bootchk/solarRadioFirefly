
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



// Hardfault logs a distinguished flag
void HardFault_Handler() { genericHardFaultHandler(); }

#ifdef NOT_USED
// This is a test harness to test handler writing to flash.
/*
 * Brownout writes PC to flash.
 * The PC will be place in main thread where power exhausted.
 * POFCON interrupt enabled for brownout detected by POFCON
 *
 * Body copied from nRF5x/exceptions/debugHandler.  See comments there.
 */
__attribute__((naked))
void HardFault_Handler(void){
/*
 * Get stack pointer that depends on mode,
 * Pass SP to a C handler.
 * This function will never return
 */

__asm(  ".syntax unified\n"
        "MOVS   R0, #4  \n"
        "MOV    R1, LR  \n"
        "TST    R0, R1  \n"
        "BEQ    _MSP    \n"
        "MRS    R0, PSP \n"
        "bl     ExceptionHandlerWritePCToFlash      \n"
        "_MSP:  \n"
        "MRS    R0, MSP \n"
		"bl     ExceptionHandlerWritePCToFlash      \n"
        ) ;
}

#endif

#ifdef OLD

".syntax divided \n"

"LDR r2, HandlerAddrConst \n"
		"B r2 \n"
		"HandlerAddrConst: .word ExceptionHandlerWritePCToFlash \n"
#endif


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



}	// extern C

