
#include <radioSoC.h>	// LEDFlasher

#include "worker.h"


/*
 * A Worker implementation where work is: flash an LED
 */



// whether work actually done
#define REAL_WORK 1

#if REAL_WORK

void Worker::workManagedAmount() {
	// Record that we worked at least once.
	// CustomFlash::writeZeroAtIndex(WorkEventFlagIndex);

	LEDFlasher::flashLEDByAmount(1, managedAmount);
}

void Worker::workAmount(unsigned int aAmount) {
	LEDFlasher::flashLEDByAmount(1, aAmount);
}


#else

void Worker::workManagedAmount() {}
void Worker::workAmount(unsigned int aAmount) {}

#endif


unsigned int maxWorkAmount() {
	return LEDFlasher::MaxFlashAmount;
}
