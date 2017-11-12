
#include "powerAdjuster.h"

// Ways of adjusting power usage
#include "../work/worker.h"
#include "../power/powerShedder.h"
#include <syncAgent/slots/fishing/fishPolicy.h>


namespace {

/*
 * A feedback loop.
 * By increasing work amount, eventually settle at a work amount that keeps voltage below excess
 * and above minimum levels.
 *
 * Parameters:
 * - how often we check (say every sync period, say every 2 seconds)
 * - how much we increment/decrement
 * - how much received work is done
 * - how fast power is changing (light levels increasing.)
 *
 * This will probably NOT work for rapid changes in power i.e. taking unit from indoors to outdoors.
 * When it doesn't work, Vcc may spike to Voc of solar cell (say >4.2V) much above Vmax (3.6V)
 *
 * Work also comes from others.
 * This should settle at the work amount proper for work from others.
 * Others should also be sending work at the same rate as self works (when not synced with others.)
 */
void increaseWorkAmountAndWorkLocally() {
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



void increasePowerUsageAndShedPower() {
	Worker::increaseAmount();
	PowerShedder::shedPowerUntilVccLessThanVmax();
	// assert power is not excess
}

} // namespace



void PowerAdjuster::setUnconstrainedEnergy(){
	// Fish more if power is unconstrained.
	SyncRecoveryTrollingPolicy::incrementFishSessionDuration(1000);
}


/*
 * Design choices:
 * Increase work amount?
 * Work locally now?
 * Send to group?
 * Work until Vcc < Vmax (closed loop feedback)
 */
void PowerAdjuster::onExcessVoltage() {

	increaseWorkAmountAndWorkLocally();

	// Optionally:
	increasePowerUsageAndShedPower();

	// Debugging
	// CustomFlash::writeZeroAtIndex(ExcessPowerEventFlagIndex);
}



void PowerAdjuster::increaseUsage() {
	Worker::increaseAmount();
}

void PowerAdjuster::decreaseUsage(){
	Worker::decreaseAmount();

}
void PowerAdjuster::maintainUsage(){
	Worker::maintainAmount();

}
void PowerAdjuster::setLeastUsage(){
	Worker::setLeastAmount();

}

