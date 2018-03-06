
#pragma once

#include <radioSoC.h>  // Mailbox, MailContents


/*
 * Work periodically, on a distributed WorkClock that is synchronized with the clique.
 * Does not work on demand.
 * Receives regular ticks on SyncPoint
 */
class WorkStrategyDistributed {
public:

	static void init(
			Mailbox* aOutMailbox,
			Mailbox* aInMailbox
	);

	/*
	 * Called at syncPoint.
	 * Tick WorkClock and possibly work.
	 * If master, tick WorkMaintenanceClock
	 */
	static void manageWorkSynchronizedDistributed();

	static void onWorkSyncMsg(MailContents work);

	static void onWorkMsg(MailContents work);
};
