
#include "worker.h"

#include <nRF5x.h>

//#include "syncAgent/flashIndex.h"


/*
 * A Worker implementation where work is: flash an LED
 *
 * Uses pure class LEDFlasher
 */

// TODO an exponential function of increasing/decreasing???

namespace {

unsigned int managedAmount = 1;

}


void Worker::init() {
	setLeastAmount();
}

// whether to work i.e. use LED's
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
 * Since we flash at sync points, we should not flash longer than a sync period,
 * else LED still on next time we want to flash.
 * Typical: sync period is 0.7 sec.
 *
 * But here we increment amount up to the limit of LEDFlasher.
 */
void Worker::increaseAmount() { if (managedAmount < LEDFlasher::MaxFlashAmount) managedAmount++; }

void Worker::maintainAmount() { } // No change to managedAmount

void Worker::decreaseAmount() { if (managedAmount > 1) managedAmount--; }

/*
 * One unit is barely perceivable flash (0.6mSec, 20 ticks).  You must be staring directly at LED.
 *
 * Amount 20 gives 12mSec flash (400 ticks), more perceivable.  But still staring directly at LED.
 *
 * Amount 100 gives 60mSec flash (2000 ticks).
 *
 * Amount 200 gives 0.12Sec, more than perceivable.
 *
 * A sync slot is say 50 ticks @ 10mA = 500 tick*mA
 * A LED flash of 2000 ticks @ 1mA (3V through 3K resistor) is 2000 tick*mA
 */
void Worker::setAmountBarelyPerceivable() { managedAmount = 1; }
void Worker::setAmountEasilyPerceivable()   { managedAmount = 200; }

void Worker::setLeastAmount() { managedAmount = 1; }

