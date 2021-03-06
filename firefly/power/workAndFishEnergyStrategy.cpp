

#include "workAndFishEnergyStrategy.h"


// sleepSyncAgent
#include <fishPolicy/slottedFishSession.h>

// TODO currently is only fish, not work

/*
 * Balanced.
 *
 * Both at same voltage (say 3V)
 * One fish slot every sync period takes 6mA for 1.5mSec => 9uW per period
 * With a 5kohm resistor and 2.1Vf LED, LED takes 0.2mA
 * One easily perceivable LED blink every 3 periods takes 0.2mA for 50mSec / 3  => 5uW per period
 * An increment to LED blink every 3 periods takes 0.2mA for 12mSec / 3  => 0.8uW per period more
 *
 * Therefore, roughly speaking,
 * incrementing fishing by one slot
 * uses ten times more energy
 * than increasing LED blink by one increment.
 */

// TODO less than one slot

void WorkAndFishEnergyStrategy::increaseUsage() {
	// Worker::increaseAmountWithoutExceedingTimerLimit();
	// TODO rename incrementFishSessionDurationBySlots
	SlottedFishSession::incrementFishSessionDuration(1);	// one slot
}

void WorkAndFishEnergyStrategy::decreaseUsage(){
	//Worker::decreaseAmount();
	SlottedFishSession::decrementFishSessionDuration(1);	// one slot
}

void WorkAndFishEnergyStrategy::maintainUsage(){
	//Worker::maintainAmount();
}

void WorkAndFishEnergyStrategy::setLeastUsage(){
	//Worker::setLeastAmount();
	SlottedFishSession::setDurationToMinDuration();
}
