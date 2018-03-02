
#pragma once

#include <services/mailbox.h>

/*
 * Work related connections between SleepSync and app.
 *
 * Encapsulates work interface, separate from SleepSync interface.
 */
class WorkFacilitator {
public:
	static void init();

	// App conveys Work or WorkSync to other units
	static Mailbox* outMailbox();

	/*
	 * WorkSync received from other units.
	 * Called in real time at unknown time in SyncSlot.
	 * Should return quickly.
	 * Do not necessarily work now, if it is time consuming.
	 */
	static void onWorkSyncMsg(MailContents);

	/*
	 * Called every SyncPeriod.
	 * Tick my clocks, generate alarms, and queue WorkSync out
	 */
	static void onSyncPoint();
};
