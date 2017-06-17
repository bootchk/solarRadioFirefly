
#include "worker.h"

#include <nRF5x.h>


/*
 * A Worker implementation where work is: flash an LED
 */

// TODO an exponential function of increasing/decreasing???

namespace {

LEDFlasher ledFlasher;

unsigned int managedAmount = 1;

}

void Worker::init(LongClockTimer* aLCT, LEDService* aLEDService) {
	ledFlasher.init(aLCT, aLEDService);
	setLeastAmount();
}

void Worker::workManagedAmount() {
	// Record that we worked at least once.
	CustomFlash::writeZeroAtIndex(WorkEventFlagIndex);

	// TEMP not actually flash
	ledFlasher.flashLEDByAmount(1, managedAmount);
}

void Worker::workAmount(unsigned int aAmount) {
	ledFlasher.flashLEDByAmount(1, aAmount);
}


/*
 * Since we flash at sync points, we should not flash longer than a sync period,
 * else LED still on next time we want to flash.
 * Typical: sync period is 0.7 sec.
 *
 * But here we increment amount up to the limit of LEDFlasher.
 */
void Worker::increaseAmount() { if (managedAmount < LEDFlasher::MaxFlashAmount) managedAmount++; }

void Worker::maintainAmount() { } // No change to managedAmount

void Worker::decreaseAmount() { if (managedAmount > 1) managedAmount--; }

// for 0.6mSec unit, 20 gives 12mSec flash, 200 gives 0.12Sec
void Worker::setAmountPerceivable() { managedAmount = 200; 	 }

void Worker::setLeastAmount() { managedAmount = 1; }

