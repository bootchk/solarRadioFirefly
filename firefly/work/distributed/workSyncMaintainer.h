
#pragma once

#include <services/mailbox.h>  // Mailbox




class WorkSyncMaintainer {
public:
	/*
	 * Init with mailbox for outgoing messages to synchronize work clocks of others in clique
	 */
	static void init(Mailbox* aOutMailbox, Mailbox* aInMailbox);

	// called from WorkClock
	static void onWorkAlarm();

	// so can check mail
	static void tickSyncPeriod();

	// callback from Provisioner
	static void onWorkSyncProvisioned(unsigned char period);

	// Need this separately for more precise work (offset to SyncPoint)
	//static void onWorkFreqProvisioned(unsigned char value);
};
