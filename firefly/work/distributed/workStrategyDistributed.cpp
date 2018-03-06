
#include "workStrategyDistributed.h"


#include "workClock.h"
#include "workSyncMaintainer.h"

#include <syncAgent/syncAgent.h>
#include <logging/logger.h>

#include <cassert>

void WorkStrategyDistributed::init(
			Mailbox* aOutMailbox,
			Mailbox* aInMailbox
	)
{
	/*
	 * WorkSyncMaintainer uses mailboxes.
	 */
	WorkSyncMaintainer::init(aOutMailbox);

	// Strategy currently not using in mail
	(void) aInMailbox;
}


void WorkStrategyDistributed::manageWorkSynchronizedDistributed() {
	// assert now is syncPoint

	// tick a clock, which might alarm, which might work and maintain work sync
	WorkClock::tickSyncPeriod();
}


void WorkStrategyDistributed::onWorkSyncMsg(MailContents work) {
	/*
	 * Handle in real time.
	 */
	WorkSyncMaintainer::onWorkSyncFromMaster(work);
}


void WorkStrategyDistributed::onWorkMsg(MailContents work) {
	// Error: this strategy should not be sending work messages
	// Harmless: just indicated older version of program
	// Or could be programming error
	Logger::log(" Distributed work received work message??\n");
	assert(false);
}


