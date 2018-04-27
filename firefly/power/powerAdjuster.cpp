

#include "powerAdjuster.h"

// Strategies of adjusting power usage
#include "workOnlyEnergyStrategy.h"
#include "workAndFishEnergyStrategy.h"


// ways.  Generally a strategy knows the ways
// but here we also know these
#include "../work/worker.h"
#include "../power/powerShedder.h"

// sleepSyncAgent
#include <fishPolicy/slottedFishSession.h>


/*
 * A feedback loop.
 * By increasing energy usage,
 * eventually settle at a usage that keeps voltage below excess and above minimum levels.
 *
 * Parameters:
 * - how often we check (say every sync period, say every 2 seconds)
 * - how much we increment/decrement
 * - how much received work is done
 * - how fast power is changing (light levels increasing.)
 *
 * This will probably NOT succeed for rapid changes in power i.e. taking unit from indoors to outdoors.
 * When it fails, Vcc may spike to Voc of solar cell (say >4.2V) much above Vmax (3.6V)
 *
 * Work also comes from others.
 * This should settle at the work amount proper for work from others.
 * Others should also be sending work at the same rate as self works (when not synced with others.)
 */


// Make strategy choice at compile time
//#define EnergyStrategy WorkerOnlyEnergyStrategy
#define EnergyStrategy WorkAndFishEnergyStrategy

namespace {



void increaseUsageAndWorkLocally() {
	/*
	 * Not assert power is not excess.
	 * Work generally drops Vcc, but is not guaranteed to drop it below Vmax.
	 */
	PowerAdjuster::increaseUsage();

	Worker::workManagedAmount();
	/*
	 * It might not make sense for others to work when we are working just to shed power.
	 * E.g. we might not be master.
	 * Here, we just work locally, meaning flash when others are not flashing.
	 * We seem to have plenty of power, and so will also be able to flash when others flash.
	 */
	// GroupWork::initiateGroupWork(WORK_VERSION);
}

#ifdef NOT_USED
#ifdef POWER_IS_SOLAR
void increasePowerUsageAndShedPower() {
	EnergyStrategy::increaseUsage();
	PowerShedder::shedPowerUntilVccLessThanVmax();
	// assert power is not excess
}
#endif
#endif

} // namespace



void PowerAdjuster::setUnconstrainedEnergy(){
	// Fish more if power is unconstrained.
	// 20 slots of 50 ticks is 1000 ticks
	SlottedFishSession::incrementFishSessionDuration(20);
}


/*
 * Design choices:
 * Increase work amount?
 * Work locally now?
 * Send to group?
 * Work until Vcc < Vmax (closed loop feedback)
 */
void PowerAdjuster::onExcessVoltage() {

	increaseUsageAndWorkLocally();

	// Optionally:
	// increasePowerUsageAndShedPower();

	// Debugging
	// CustomFlash::writeZeroAtIndex(ExcessPowerEventFlagIndex);
}




void PowerAdjuster::increaseUsage() {
	EnergyStrategy::increaseUsage();
}

void PowerAdjuster::decreaseUsage(){
	EnergyStrategy::decreaseUsage();
}

void PowerAdjuster::maintainUsage(){
	EnergyStrategy::maintainUsage();
}

void PowerAdjuster::setLeastUsage(){
	EnergyStrategy::setLeastUsage();
}

