
#include <radioSoC.h>	// LEDFlasher

#include "worker.h"


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
#define REAL_WORK 0
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
 * Here, to amount that is easily visible (the initial value.)
 * If power is low, we won't be trying work.
 */
void Worker::setLeastAmount() {
	setManagedAmount(AmountEasilyPerceivable);
}

/*
 * Since we work at sync points, we should not work (flash) longer than a sync period,
 * else work still being done at next time we want to work.
 * Typical: sync period is a second.
 *
 * But here we increment amount up to the limit of LEDFlasher.
 */
void Worker::increaseAmountWithoutExceedingTimerLimit() {
	if (managedAmount < LEDFlasher::MaxFlashAmount) managedAmount += ChangeAmount;
}

void Worker::decreaseAmount() {
	if (managedAmount > ChangeAmount) managedAmount -= ChangeAmount;
}



void Worker::maintainAmount() { } // No change to managedAmount

void Worker::setManagedAmount(unsigned int amount) { managedAmount = amount; }



