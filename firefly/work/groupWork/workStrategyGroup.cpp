
#include <modules/syncPowerManager.h>
#include <logging/flashIndex.h>
#include <syncAgent/syncAgent.h>

// embeddedMath
#include <random.h>


#include "workStrategyGroup.h"
#include "groupWork.h"

#include "../workFactory.h"
#include "../worker.h"

#include "../parameters/workFrequency.h"




namespace {
unsigned int regularWorkCounter = 0;
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
void WorkStrategyGroup::manageWorkOnlyRandomlyIfPowerAndMaster() {
	if (SyncAgent::isSelfMaster() and SyncPowerManager::isPowerForWork()) {
		// random duration between work
		doRandomWork();
	}
}


void WorkStrategyGroup::manageWorkOnlyRegularlyIfPowerAndMaster() {
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



/*
 * Can be called every SyncCallback for testing when not solar powered.
 *
 * When solar powered, only called when there is power.
 */
void WorkStrategyGroup::doRandomWork() {
	if (randomProbability(WorkFrequency::syncPeriodsBetweenWork())) {
		GroupWork::initiateGroupWork(WorkFactory::make());
	}
}

void WorkStrategyGroup::doRegularWork() {
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

void WorkStrategyGroup::doRegularLocalWork() {
	Worker::workManagedAmount();
}


void WorkStrategyGroup::onWorkMsg(MailContents work) {
	// Queue work to be done later (at next sync point)
	GroupWork::queueLocalWork(work);
}

