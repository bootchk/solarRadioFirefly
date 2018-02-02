

#include "workAmount.h"
#include "worker.h"

// TODO an exponential function of increasing/decreasing???

namespace {

unsigned int managedAmount = 1;

}


void WorkAmount::init(unsigned int initialAmount) {
	setManagedAmount(initialAmount);
}

/*
 * Here, to amount that is easily visible (the initial value.)
 * If power is low, we won't be trying work.
 */
void WorkAmount::setLeastAmount() {
	setManagedAmount(AmountEasilyPerceivable);
}

/*
 * Since we work at sync points, we should not work (flash) longer than a sync period,
 * else work still being done at next time we want to work.
 * Typical: sync period is a second.
 *
 * But here we increment amount up to the limit of LEDFlasher.
 */
void WorkAmount::increaseAmountWithoutExceedingTimerLimit() {
	if (managedAmount < Worker::maxWorkAmount()) managedAmount += ChangeAmount;
}

void WorkAmount::decreaseAmount() {
	if (managedAmount > ChangeAmount) managedAmount -= ChangeAmount;
}



void WorkAmount::maintainAmount() { } // No change to managedAmount

void WorkAmount::setManagedAmount(unsigned int amount) { managedAmount = amount; }
