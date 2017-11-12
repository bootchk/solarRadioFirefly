

#include "../work/worker.h"

#include "workOnlyEnergyStrategy.h"


void WorkOnlyEnergyStrategy::increaseUsage() {
	Worker::increaseAmountWithoutExceedingTimerLimit();
}

void WorkOnlyEnergyStrategy::decreaseUsage(){
	Worker::decreaseAmount();
}

void WorkOnlyEnergyStrategy::maintainUsage(){
	Worker::maintainAmount();
}

void WorkOnlyEnergyStrategy::setLeastUsage(){
	Worker::setLeastAmount();
}
