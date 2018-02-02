

#include "../work/workAmount.h"

#include "workOnlyEnergyStrategy.h"


void WorkOnlyEnergyStrategy::increaseUsage() {
	WorkAmount::increaseAmountWithoutExceedingTimerLimit();
}

void WorkOnlyEnergyStrategy::decreaseUsage(){
	WorkAmount::decreaseAmount();
}

void WorkOnlyEnergyStrategy::maintainUsage(){
	WorkAmount::maintainAmount();
}

void WorkOnlyEnergyStrategy::setLeastUsage(){
	WorkAmount::setLeastAmount();
}
