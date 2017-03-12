
#include "worker.h"

#include <nRF5x.h>


/*
 * A Worker implementation where work is: flash an LED
 */

namespace {

LEDFlasher ledFlasher;

unsigned int amount = 1;

}

void Worker::init(LongClockTimer* aLCT, LEDService* aLEDService) {
	ledFlasher.init(aLCT, aLEDService);
	setLeastAmount();
}

void Worker::work() {
	ledFlasher.flashLEDByAmount(1, amount);
}

void Worker::setLeastAmount() {
	amount = 1;
}
void Worker::increaseAmount() {
	/*
	 * Since we flash at sync points,
	 * we should not flash longer than a sync period,
	 * else LED still on next time we want to flash.
	 *
	 * Typical: sync period is 0.7 sec.
	 *
	 * But here we increment amount up to the limit of LEDFlasher.
	 */

	if (amount < LEDFlasher::MaxFlashAmount) amount++;
}

void Worker::decreaseAmount() {
	if (amount > 1) amount--;
}
void Worker::setAmountPerceivable() {
	amount = 200; 	// for 0.6mSec unit, 20 gives 12mSec flash, 200 gives 0.12Sec
}

