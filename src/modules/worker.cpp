
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
	if (amount < LEDFlasher::MaxFlashAmount) amount++;
}
void Worker::decreaseAmount() {
	if (amount > 1) amount--;
}
void Worker::setAmountPerceivable() {
	amount = 200; 	// for 0.6mSec unit, 20 gives 12mSec flash, 200 gives 0.12Sec
}

