
// Config depends on POWER_IS_SOLAR in build config


//#include <services/logger.h>	// RTTLogger
//#include <syncAgent/syncAgent.h>

#include "workSupervisor.h"

// Uses
#include <syncAgent/modules/syncPowerManager.h>
#include <syncAgent/logging/logger.h>
#include "worker.h"
#include "groupWork.h"
#include "powerShedder.h"
#include "workStrategy.h"



/*
 * Implementation notes:
 *
 * Uses a WorkStrategy (choice of many alternatives.)
 * (adjusts workAmount and works to manage power)
 */


void WorkSupervisor::init(
		Mailbox* aOutMailbox,
		Mailbox* aInMailbox
		)
{
	//Worker::init(Worker::AmountBarelyPerceivable);
	Worker::init(Worker::AmountEasilyPerceivable);
	// AmountMoreThanPerceivable

	// self doesn't use mailbox, merely passes mailbox to groupWorker
	// GroupWork also uses Worker
	GroupWork::init(aOutMailbox, aInMailbox);

	// SyncPowerManager needs no init
}



/*
 * This is called at SyncPoint (but it may be drifted.)
 *
 * This also determines the behaviour of the flashing:
 * - units try to flash in sync
 * - units with excess power may flash when other units can't
 * - when all units have moderate power, some unit may randomly trigger all to flash
 *
 */
void WorkSupervisor::manageVoltageAndWork() {

/*
 * Choose a WorkStrategy peculiar to my test HW:
 *
 * 1) nRF52DK, with regulated, line power, thus there is always enough power
 * and never too much voltage: no need to manage voltage.
 *
 * 2) nRF51 with unregulated solar power.
 * Depending on the light, the voltage may exceed Vmax 3.6V of the unit,
 * so this algorithm attempts to use power to control voltage.
 * The solar cells can produce a max of 4.8V, which is not exceedingly dangerous to the chips.
 *
 * 3) nRF51 with unregulated but constrained solar power.
 * The solar cells cannot produce >3.6V in any light level.
 */
#ifdef POWER_IS_SOLAR
	// Solar cell Voc exceed SoC Vmax
	// WorkStrategy::simpleManagePowerWithWork();

	// Solar cell Voc always below SoC Vmax (no need to shed power.)
	// WorkStrategy::manageWorkOnlyRandomlyIfPowerAndMaster();

	WorkStrategy::manageWorkOnlyRegularlyIfPowerAndMaster();

#else
	/*
	 * Choices here for testing, when power supply is steady
	 */
	// this is group work initiated by non-masters
	//WorkStrategy::doRandomWork();

	// Group work initiated only by master
	//WorkStrategy::manageWorkOnlyRandomlyIfPowerAndMaster();

	// Work regardless of mastership or power
	// This will work even while sync is just being maintained (radio not in use)
	// until brownout occurs.
	WorkStrategy::doRegularLocalWork();
#endif


}


void WorkSupervisor::tryWorkLocally() {
	if (SyncPowerManager::isPowerForWork()) {
		Logger::log("App works.");
		Worker::workManagedAmount();
	}
	else {
		// omit work, not enough power
		Logger::log("\nNo power to work.");
	}

}


void WorkSupervisor::queueLocalWork(WorkPayload work) {
	GroupWork::queueLocalWork(work);
}

