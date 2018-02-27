
#pragma once

#include <radioSoC.h>  // Mailbox, MailContents


class WorkStrategyDistributed {
public:

	static void init(
			Mailbox* aOutMailbox,
			Mailbox* aInMailbox
	);

	/*
	 * Work periodically on a schedule that is synchronized with the clique,
	 * but doesn't require a "work now" from the master,
	 * only message from master to synchronize the work schedule.
	 */
	static void manageWorkSynchronizedDistributed();

	static void provisionWorkTime(unsigned int periodsElapsed);

	static void onWorkMsg(MailContents work);
};
