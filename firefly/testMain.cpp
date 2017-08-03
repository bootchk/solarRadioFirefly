
#include <cassert>
#include <inttypes.h>

#include "nrf.h"	// WEV

#include "nRF5x.h"
#include "/home/bootch/git/nRF5x/src/services/customFlash.h"

void testMain();


/*
 * Main for testing library routines.
 *
 * Comment out old tests.
 */
void testMain() {

	/*
	 * April 2017 Test various faults leave a trace in FLASH (UICR)
	 * and put cpu in permanent sleep.
	 *
	 * Expect to end
	 */

	/*
	 * Write to odd address.
	 * This will succeed on newer ARM architectures that support unaligned access.
	 * Hence it does not test HardFault
	 */
	//*(uint32_t *)(1) = 0;

	/*
	 * Test HardFault on reading invalid address
	 * x20010000 is beyond the last word of RAM (64k of RAM starting at x2000 0000
	 */
	//int foo = *(uint32_t *)(0x20010004);

	/*
	 * Test assertion wites to flash,
	 * since redefined __assert_func() in handlers.cpp calls genericAssertionFaultHandler()
	 */
	//assert(false);


	/*
	 * Test writing UICR
	 */
	//CustomFlash::writeZeroAtIndex(ExitFlagIndex);

	/*
	 * Test DebugHandler writes PC to flash
	 * When hardfault occurs
	 * (Since HardFault_Handler is alias for DebugHandler (asm) which calls ExceptionHandlerWritePCToFlash
	 */
	//int foo = *(uint32_t *)(0x20010004);	// addr out of range

	/*
	 * Test WFE, SEV sequence
	 *
	 * Determined that arm gdb will not step through this???
	 */
	while(true) {
		__SEV();
		__WFE();
		__WFE();
	}
}
