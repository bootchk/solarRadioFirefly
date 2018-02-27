
// Config depends on POWER_IS_SOLAR in build config

//#include <syncAgent/syncAgent.h>

#include "workSupervisor.h"

// sleepSyncAgent
#include <modules/syncPowerManager.h>
#include <logging/logger.h>


#include <work/groupWork/workStrategyGroup.h>
#include <work/distributed/workStrategyDistributed.h>

#include "groupWork/groupWork.h"
#include "worker.h"
#include "parameters/workAmount.h"


// For distributed worker strategy
#include "distributed/distributedSynchronizedWorker.h"
#include "distributed/workSyncMaintainer.h"
//#include "distributed/workClock.h"

// radioSoC
//#include <services/logger.h>	// RTTLogger


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
	//WorkAmount::init(WorkAmount::AmountBarelyPerceivable);
	WorkAmount::init(WorkAmount::AmountEasilyPerceivable);
	// AmountMoreThanPerceivable

	// SyncPowerManager needs no init
	// Worker and DistributedSynchronizedWorker need no init

	/*
	 * Self doesn't use mailbox, merely passes mailbox to other objects,
	 * who may use the mailboxes for different purposes.
	 *
	 * GroupWork uses it for WorkNow messages
	 * WorkSyncMaintainer uses it for WorkSync messages.
	 */

	// Choice here

	// 1
	//GroupWork::init(aOutMailbox, aInMailbox);
	// 2
	WorkSyncMaintainer::init(aOutMailbox, aInMailbox);
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

#ifdef SOLAR_EXCEEDS_VMAX
	// Solar cell Voc exceed SoC Vmax
	WorkStrategy::manageExcessPowerWithWork();
#else
	// Solar cell Voc always below SoC Vmax (no need to shed power.)
	// WorkStrategy::manageWorkOnlyRandomlyIfPowerAndMaster();
	// WorkStrategy::manageWorkOnlyRegularlyIfPowerAndMaster();

	// TEMP testing energy strategy on DK
	WorkStrategy::manageExcessPowerWithWork();
#endif

#else
	/*
	 * Choices here.
	 * For testing with DK or battery, when power supply is steady
	 */
	// Group work initiated by non-masters
	//WorkStrategy::doRandomWork();

	// Group work initiated only by master
	//WorkStrategy::manageWorkOnlyRandomlyIfPowerAndMaster();

	// Local work regardless of mastership or power
	// This will work even while sync is just being maintained (radio not in use)
	// until brownout occurs.
	// WorkStrategy::doRegularLocalWork();

	// Group work
	// WorkStrategy::manageWorkOnlyRegularlyIfPowerAndMaster();

	// Distributed worker, works regardless of power
	WorkStrategyDistributed::manageWorkSynchronizedDistributed();
#endif


}


void WorkSupervisor::tryWorkLocally() {
	if (SyncPowerManager::isPowerForWork()) {
		Logger::log(" App LED.");
		Worker::workManagedAmount();
	}
	else {
		// omit work, not enough power
		Logger::log("\nNo power to work.");
	}

}


void WorkSupervisor::queueLocalWork(MailContents work) {
	GroupWork::queueLocalWork(work);
}


/*
 * Called at provisioning time.
 * Real provisioning (by user, the "mark" on wall clock) happened at offset from now.
 *
 * Calculate how many sync periods have elapsed since then and sync my workClock to that wall time.
 * TODO
 *
 * Note there is unused design to work at a syncPeriodOffset from syncPoint.
 * Here, we always work at a syncPoint.
 * Thus separate clique's work is not synchronized yet.
 */
void WorkSupervisor::provisionWork() {
	// Delegate to WorkStrategy

	// 1
	// TODO WorkStrategyGroup::provisionWork();
	// 2
	WorkStrategyDistributed::provisionWork();
}


