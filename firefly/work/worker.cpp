
#include <radioSoC.h>	// LEDFlasherTask

#include "worker.h"
#include "parameters/workAmount.h"
#include "parameters/workTime.h"


/*
 * A Worker implementation where work is: flash an LED
 */


namespace {

void workNow() {
	/*
	 * Choice here: old implementation using interrupts, or not.
	 */
	//LEDFlasher::flashLEDByAmount(1, WorkAmount::managedAmount());
	LEDFlasherTask::flashLEDByAmount(WorkAmount::managedAmount());
}

#ifdef NOT_USED
needed if work at offset in syncperiod
void workInFuture() {
	LEDFlasher::scheduleFlashLEDByAmount(1,
				WorkAmount::managedAmount(),
				WorkTime::periodTimeToWork());
}
#endif
}




// whether work actually done
#define REAL_WORK 1

#if REAL_WORK

void Worker::workManagedAmount() {
	// Record that we worked at least once.
	// CustomFlash::writeZeroAtIndex(WorkEventFlagIndex);

	/*
	 * Choice here: work when called, or later
	 */
	workNow();
	//workInFuture();
}


void Worker::workAmount(unsigned int aAmount) {
	//LEDFlasher::flashLEDByAmount(1, aAmount);
	LEDFlasherTask::flashLEDByAmount(aAmount);
}


#else

void Worker::workManagedAmount() {}
void Worker::workAmount(unsigned int aAmount) {}

#endif


unsigned int Worker::maxWorkAmount() {
	return LEDFlasherTask::MaxFlashAmount;

}
