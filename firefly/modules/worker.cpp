
#include "worker.h"

#include <nRF5x.h>	// LEDFlasher


/*
 * A Worker implementation where work is: flash an LED
 *
 * Uses pure class LEDFlasher
 */

// TODO an exponential function of increasing/decreasing???

namespace {

unsigned int managedAmount = 1;

}


void Worker::init(unsigned int initialAmount) {
	setManagedAmount(initialAmount);
}

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


/*
 * Since we work at sync points, we should not work longer than a sync period,
 * else work still being done at next time we want to work.
 * Typical: sync period is 0.7 sec.
 *
 * But here we increment amount up to the limit of LEDFlasher.
 */
void Worker::increaseAmount() { if (managedAmount < LEDFlasher::MaxFlashAmount) managedAmount++; }

void Worker::maintainAmount() { } // No change to managedAmount

void Worker::decreaseAmount() { if (managedAmount > 1) managedAmount--; }

void Worker::setManagedAmount(unsigned int amount) { managedAmount = amount; }



