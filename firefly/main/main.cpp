

#include "mainTask.h"

// radioSoC
#include <clock/mcuSleep.h>
#include <exceptions/powerAssertions.h>


// Alternative mains
void testMain();
//void powerManagedMain() __attribute__ ((noreturn));


// Doesn't return, but compiler requires it to return int
// so get a spurious warning "has a return statement."

//__attribute__((noreturn))


#include "nrf_soc.h"	// sd_app_evt_wait

void sleep();

#ifdef SOFTDEVICE_PRESENT
/*
 * Sequential multi-protocol: sleep implement depends on active protocol (BLE or SleepSync)
 */
// libBLEProvisionee
#include <provisioner.h>

void sleep() {
	if (Provisioner::isProvisioning()) {
		// Fails to link: MCUSleep::untilInterruptSDCompatible();
		uint32_t errorCode = sd_app_evt_wait();
		// always return NRF_SUCCESS, according to docs
		(void) errorCode;
	}
	else {
		MCUSleep::untilInterrupt();

	}
}
#else
void sleep() {
	// MCUSleep::untilAnyEvent();
	MCUSleep::untilInterrupt();
}
#endif




int main() {
	// assert embedded system startup is done.  SoC in reset state.

	// Choose alternative main
	// testMain();	// test harness
	// powerManagedMain();	// old design using sleep
    // or use RTC Task design as follows:

	MainTask::onReset();

	MainTask::scheduleCheckPowerReserve();
	/*
	 * If ever power reserves are met (depends on solar power supply)
	 * the checkPowerReserve task will schedule startup tasks of SyncAgent loop on syncPeriods
	 * including a callback to onMuchPowerReserve()
	 */


	/*
	 * loop on interrupts:
	 * - scheduled tasks (Timer ISR)
	 * - tasks from concurrent devices (device ISR's)
	 */

	while (true) {
		sleep();

		/*
		 * An ISR has completed.
		 * Assert another interrupt will come (Timer scheduled or device is concurrently processing, set to interrupt.)
		 */
		/*
		 * Often, but not always, all devices off after a task completes.
		 * Make assertions on power in the tasks that turn off devices.
		 */
	}

}
