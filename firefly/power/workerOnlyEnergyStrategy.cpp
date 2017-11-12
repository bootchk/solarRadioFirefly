
#include "workerOnlyEnergyStrategy.h"

#include "../work/worker.h"

void WorkerOnlyEnergyStrategy::increaseUsage() {
	Worker::increaseAmountWithoutExceedingTimerLimit();
}

void WorkerOnlyEnergyStrategy::decreaseUsage(){
	Worker::decreaseAmount();
}

void WorkerOnlyEnergyStrategy::maintainUsage(){
	Worker::maintainAmount();
}

void WorkerOnlyEnergyStrategy::setLeastUsage(){
	Worker::setLeastAmount();
}
