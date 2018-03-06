
#pragma once

#include <services/mailbox.h>  // Mailbox




class WorkSyncMaintainer {
public:
	/*
	 * mailbox for outgoing messages to synchronize work clocks of others in clique
	 */
	static void init(Mailbox* aOutMailbox);

	// from WorkClock
	static void onWorkAlarm();

	// from WorkSyncMaintenanceClock
	static void onWorkSyncMaintenanceAlarm();

	//
	static void onWorkSyncFromMaster(unsigned char);

	// callback from Provisioner
	static void onWorkSyncProvisioned(unsigned char period);

	// Need this separately for more precise work (offset to SyncPoint)
	//static void onWorkFreqProvisioned(unsigned char value);


};
