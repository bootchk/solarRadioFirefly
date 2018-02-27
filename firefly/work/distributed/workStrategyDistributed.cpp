
#include "workStrategyDistributed.h"


#include "workClock.h"
#include "workSyncMaintainer.h"

#include <syncAgent/syncAgent.h>



void WorkStrategyDistributed::manageWorkSynchronizedDistributed() {
	// assert now is syncPoint

	// tick a clock, which might alarm, which might work and maintain sync
	WorkClock::tickSyncPeriod();

	if (SyncAgent::isSelfSlave())
		WorkSyncMaintainer::checkForWorkSyncFromMaster();
}


void WorkStrategyDistributed::provisionWork() {

	// The provisioning does not carry a workCycle, so set it to the current one.
	WorkClock::setSync(WorkClock::getPeriod());
}
