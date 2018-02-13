
#include "workSyncMaintainer.h"
#include "workClock.h"

#include <syncAgent/syncAgent.h>
#include <cassert>



namespace {

/*
 * Counter runs free, wraps without harm
 * Period does not need to be power of two.
 * When it wraps, we might get an extra alarm,
 * but it is in sync with the WorkClock.
 */
unsigned int _counter = 0;

unsigned char _workMaintenancePeriod = 5;

Mailbox * _inBox;
Mailbox * _outBox;


void onTickWorkMaintenanceAlarm() {
	/*
	 * Periodic alarm has gone off.
	 * !!! And self is working (WorkClock has alarmed also.)
	 *
	 * Tell clique members:
	 * - sync: now is work time
	 * - period: of WorkClock (not of WorkMaintenance clock.)
	 *
	 * Both might have been changed by provisioning
	 * since last time I told clique members.
	 */
	_outBox->put(WorkClock::period());
}

/*
 * Tick my software WorkMaintenanceClock
 */
void tickCounter() {
	_counter++;

	// 0 is arbitrary choice
	if ( (_counter % _workMaintenancePeriod) == 0) {
		onTickWorkMaintenanceAlarm();
	}
}

bool isPowerOfTwo(unsigned char x) {
	// See Hacker's delight or SO
	return (x != 0) && ((x & (x - 1)) == 0);
}
}

void WorkSyncMaintainer::init(Mailbox* aOutMailbox, Mailbox* aInMailbox) {
	_inBox = aInMailbox;
	_outBox = aOutMailbox;
}


/*
 * My clock gets ticks from WorkClock (less frequent than tickSyncPeriod).
 * I.E. I am working now.
 */

void WorkSyncMaintainer::onWorkAlarm() {
	// Only a Master maintains sync for others
	if (SyncAgent::isSelfMaster())
		tickCounter();
}


/*
 * Check for WorkSync every syncPeriod so Work is synchronized to nearest syncPoint
 */
void WorkSyncMaintainer::tickSyncPeriod() {
	/*
	 * Only slaves should receive WorkSync (Masters send, can't receive their own.)
	 */
	assert( ! SyncAgent::isSelfMaster() );

	if (_inBox->isMail()) {
		/*
		 * Master telling self to sync local clock.
		 */
		unsigned char newPeriod = _inBox->fetch();

		/*
		 * Ensure period is power of 2
		 */
		assert(isPowerOfTwo(newPeriod));

		WorkClock::setSync(newPeriod);

	}
}



void WorkSyncMaintainer::onWorkSyncProvisioned(unsigned char period) {
	/*
	 * Provisioning has same semantics as what I tell other clique members:
	 * - sync: clock starts now
	 * - period
	 */
	// Only Master should receive this callback.
	assert( SyncAgent::isSelfMaster() );

	/*
	 * Set local WorkClock.  As Master, tell other clique members soon.
	 */
	WorkClock::setSync(period);
}

