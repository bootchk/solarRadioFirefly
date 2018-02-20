
// sleepSyncAgent
#include <modules/syncPowerManager.h>
#include <logging/flashIndex.h>

#include <random.h>
#include <syncAgent/syncAgent.h>

#include "workFactory.h"
#include "groupWork.h"
#include "worker.h"
#include "workStrategy.h"

#include "distributed/workClock.h"
#include "distributed/workSyncMaintainer.h"

#include "parameters/workFrequency.h"

#include "../power/powerAdjuster.h"




namespace {
unsigned int regularWorkCounter = 0;


void doNestedWorkStrategy() {
	/*
	 * Choice here of nested work strategy.
	 * Similar to choice when outer strategy does not manage excess voltage.
	 */
	WorkStrategy::manageWorkOnlyRegularlyIfPowerAndMaster();
}


}	// namespace




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
void WorkStrategy::manageWorkOnlyRandomlyIfPowerAndMaster() {
	if (SyncAgent::isSelfMaster() and SyncPowerManager::isPowerForWork()) {
		// random duration between work
		doRandomWork();
	}
}


void WorkStrategy::manageWorkOnlyRegularlyIfPowerAndMaster() {
	if (SyncAgent::isSelfMaster() and SyncPowerManager::isPowerForWork()) {
		// Fixed duration between work
		doRegularWork();
	}
}


/*
 * Strategy that avoids excess voltage AND tries to work according to a nested strategy.
 *
 * Since getVoltageRange uses ADC on some platforms,
 * it might take >20uSec (>one tick) and should not be called at SyncPoint
 *
 * Here, if voltage is climbing, do more work.
 * Note work is asynchronous, more work does not require more time in this routine.
 * A very simple approach, not very effective?
 */
// TODO replace this with a state machine that is more effective

void WorkStrategy::manageExcessPowerWithWork() {

	/* It is possible for power levels to drop so precipitously that
	 * on consecutive calls, the VoltageRange's are not adjacent,
	 * i.e. pass from Excess to Medium or worse.
	 */
	switch (SyncPowerManager::getVoltageRange()) {

	case VoltageRange::AboveExcess: // e.g. > 3.6V
	case VoltageRange::NearExcess: // e.g. > 3.4V
		PowerAdjuster::onExcessVoltage();
		break;

	/*
	 * Rest of cases:
	 * Adjust work amount.
	 * Self not need to work to manage excess power.
	 * Delegate to other work strategy to decide whether to work.
	 *
	 *
	 * Delegated work strategy may cause local work.
	 * Thus we might do two local works in consecutive calls.
	 * I.E. For Excess, then for High
	 */

	/* Targeted range, i.e. moderate. */
	case VoltageRange::HighToNearExcess:  // e.g. 2.7--3.4V
	case VoltageRange::MediumToHigh:   // e.g. 2.5--2.7V
		PowerAdjuster::maintainUsage();
		doNestedWorkStrategy();
		// WAS GroupWork::initiateGroupWork(WORK_VERSION);
		break;


	case VoltageRange::LowToMedium:  // e.g. 2.3-2.5V
		PowerAdjuster::decreaseUsage();
		doNestedWorkStrategy();
		break;


	case VoltageRange::UltraLowToLow:	// e.g. 2.1-2.3V
		/*
		 * Not enough power to work, and power getting low.
		 */
		PowerAdjuster::decreaseUsage();
		break;

	case VoltageRange::BelowUltraLow:	//  e.g. <2.1V
		/*
		 * Not enough power to work.
		 * Power nearing brownout.
		 * Next work done (if ever) is least amount.
		 *
		 * It is possible for power levels to drop so precipitously that
		 * we didnt' pass through case above for Low.
		 */
		/* See also brownout detector should find this case asynchronously. */
		PowerAdjuster::setLeastUsage();
		break;
	}
}


/*
 * Can be called every SyncCallback for testing when not solar powered.
 *
 * When solar powered, only called when there is power.
 */
void WorkStrategy::doRandomWork() {
	if (randomProbability(WorkFrequency::syncPeriodsBetweenWork())) {
		GroupWork::initiateGroupWork(WorkFactory::make());
	}
}

void WorkStrategy::doRegularWork() {
	if (regularWorkCounter > WorkFrequency::syncPeriodsBetweenWork()) {
		GroupWork::initiateGroupWork(WorkFactory::make());
		regularWorkCounter = 0;
	}
	regularWorkCounter++;

	/*
	 * Loosely: assert(regularWorkCounter <  WorkFrequency::syncPeriodsBetweenWork());
	 * But since syncPeriodsBetweenWork() can change at any time to a smaller value, can't assert.
	 */
}

void WorkStrategy::doRegularLocalWork() {
	Worker::workManagedAmount();
}

void WorkStrategy::manageWorkSynchronizedDistributed() {
	// assert now is syncPoint

	// tick a clock, which might alarm, which might work and maintain sync
	WorkClock::tickSyncPeriod();

	if (SyncAgent::isSelfSlave())
		WorkSyncMaintainer::checkForWorkSyncFromMaster();
}
