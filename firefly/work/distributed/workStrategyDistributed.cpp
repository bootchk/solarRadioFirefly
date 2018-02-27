
#include "workStrategyDistributed.h"


#include "workClock.h"
#include "workSyncMaintainer.h"

#include <syncAgent/syncAgent.h>
#include <logging/logger.h>



void WorkStrategyDistributed::init(
			Mailbox* aOutMailbox,
			Mailbox* aInMailbox
	)
{
	/*
	 * WorkSyncMaintainer uses mailboxes.
	 */
	WorkSyncMaintainer::init(aOutMailbox, aInMailbox);
}


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

	// TODO use the offset and calculate what former sync period user pushed button
	// i.e. adjust workclock forward or back, not just to 0
}


void WorkStrategyDistributed::onWorkMsg(MailContents work) {
	// Error: this strategy should not be sending work messages
	// Harmless: just indicated older version of program
	// Or could be programming error
	Logger::log(" Distributed work received work message??\n");

}
