
#include "workStrategy.h"

// Uses these pure class
#include <syncAgent/modules/syncPowerManager.h>
#include "worker.h"
#include "groupWork.h"
#include "powerShedder.h"

// Flags
#include <syncAgent/logging/flashIndex.h>



// dummy workpayload represents version of code
#define WORK_VERSION 34


/* WorkStrategy can conflate two things:
 * - adjusting workAmount if power permits more work
 * - adjusting power by working
 * In other words, managing system voltage excess (high side)
 * and managing system voltage deficit (low side.)
 *
 * Part of the total voltage/power management.
 * This is high side.
 * See also SyncSleep, which also does power management on the low side.
 */


/*
 * Strategy that does NOT prevent excess voltage,
 * and does a fixed amount of work (no adjusting workAmount)
 * but does no work when power is below a minimum.
 *
 * This should be faster than other strategies.
 * So it can be called at SyncPoint without disrupting sync???
 */
void WorkStrategy::manageWorkOnly() {
	if ( SyncPowerManager::isPowerForWork()) {
		GroupWork::randomlyInitiateGroupWork(WORK_VERSION);
	}
}



/*
 * Strategy that avoids excess voltage AND tries to work as much as possible.
 *
 * Since it uses ADC, takes much time and should not be called at SyncPoint
 *
 * Here, if voltage is climbing, do more work.
 * Note work is asynchronous, more work does not require more time in this routine.
 * A very simple approach, not very effective?
 */
// TODO replace this with a state machine that is more effective

void WorkStrategy::simpleManagePowerWithWork() {

	/* It is possible for power levels to drop so precipitously that
	 * on consecutive calls, the VoltageRange's are not adjacent,
	 * i.e. pass from Excess to Medium or worse.
	 */
	switch (SyncPowerManager::getVoltageRange()) {
	case VoltageRange::AboveExcess:
		/* e.g. > 3.6V.
		 * Self MUST work locally to keep voltage from exceeding Vmax
		 * Send to group.
		 */
		Worker::increaseAmount();
		GroupWork::initiateGroupWork(WORK_VERSION);

		CustomFlash::writeZeroAtIndex(ExcessPowerEventFlagIndex);

		PowerShedder::shedPowerUntilVccLessThanVmax();
		// assert power is not excess?
		// TODO if change ShedPower, Vcc might still be excess
		break;

	case VoltageRange::HighToExcess:
		/* e.g. 2.7--3.6V
		 * I could work (enough power) but I don't need to work to manage excess power.
		 */
		Worker::maintainAmount();

		/*
		 * This may cause local work.
		 * Thus we might do two local works in consecutive calls.
		 * I.E. For Excess, then for High
		 * I.E. For High, then High (when both random results are true.)
		 */
		GroupWork::randomlyInitiateGroupWork(WORK_VERSION);
		break;

	case VoltageRange::MediumToHigh:
		// e.g. 2.5--2.7V
		Worker::maintainAmount();
		break;

	case VoltageRange::LowToMedium:
		/* e.g. 2.3-2.5V
		 * not initiate work.
		 * Others may ask me to work, see elsewhere.
		 */
		Worker::decreaseAmount();
		break;

	case VoltageRange::UltraLowToLow:
		/* e.g. 2.1-2.3V
		 * Not enough power to work, and power getting low.
		 * Next work done (if ever) is least amount.
		 */
		Worker::setLeastAmount();
		break;

	case VoltageRange::BelowUltraLow:
		/* e.g. <2.1V
		 * Not enough power to work.
		 * Power nearing brownout.
		 * Next work done (if ever) is least amount.
		 *
		 * It is possible for power levels to drop so precipitously that
		 * we didnt' pass through case above for Low.
		 */
		/* See also brownout detector should find this case asynchronously. */
		Worker::setLeastAmount();
		break;
	}
}


/*
 * For testing, when not solar powered.
 * Periodically, when called back by SyncAgent, initiate work.
 */
void WorkStrategy::doRandomWork() {
	//Default amount of work is enough to perceive, but make it more visible...
	Worker::setAmountPerceivable();

	GroupWork::randomlyInitiateGroupWork(WORK_VERSION);
}
