
#include "workSyncMaintainer.h"

#include "workClock.h"
#include "workSyncMaintenanceClock.h"
#include "../parameters/workFrequency.h"

#include <syncAgent/syncAgent.h>
#include <cassert>

// radioSoC library
#include <services/logger.h>


namespace {

Mailbox * _inBoxForWorkSync;
Mailbox * _outBoxForWorkSync;


bool isPowerOfTwo(unsigned char x) {
	// See Hacker's delight or SO
	return (x != 0) && ((x & (x - 1)) == 0);
}

} // namespace




void WorkSyncMaintainer::init(Mailbox* aOutMailbox, Mailbox* aInMailbox) {
	_inBoxForWorkSync = aInMailbox;
	_outBoxForWorkSync = aOutMailbox;
}


/*
 * My clock gets ticks from WorkClock.
 * I.E. I am working now.
 * Tick a slower clock for WorkSyncMaintenance.
 */
void WorkSyncMaintainer::onWorkAlarm() {
	// Only a Master maintains sync for others
	if (SyncAgent::isSelfMaster())
		WorkSyncMaintenanceClock::tick();
}


/*
 * Check for WorkSync every syncPeriod so Work is synchronized to nearest syncPoint
 */
// TODO this is at the syncpoint, one sync period late?
void WorkSyncMaintainer::checkForWorkSyncFromMaster() {
	/*
	 * Only slaves receive WorkSync (Masters send, can't receive their own.)
	 * Master updates its WorkClock (the master workClock, when provisioned.)
	 */
	assert( SyncAgent::isSelfSlave() );

	if (_inBoxForWorkSync->isMail()) {
		unsigned char newPeriod = _inBoxForWorkSync->fetch();

		/*
		 * Ensure period is power of 2
		 */
		assert(isPowerOfTwo(newPeriod));

		WorkClock::syncToNow(newPeriod);
	}
}

// TODO no caller?

// TODO only set the period, and leave the sync alone
// i.e. at the next work alarm, change the period for the subsequent work alarms.
// shortening and lengthening takes care
void WorkSyncMaintainer::onWorkSyncProvisioned(unsigned char period) {
	/*
	 * Provisioning has same semantics as what I tell other clique members:
	 * - sync: clock starts now
	 * - period
	 */
	// Only Master should receive this callback.
	assert( SyncAgent::isSelfMaster() );

	/*
	 * Set local, master WorkClock.
	 * As Master, tell other clique members of new clock value later,
	 * when next the WorkSyncMaintence clock alarms.
	 */
	WorkClock::syncToNow(period);
}

void WorkSyncMaintainer::onWorkSyncMaintenanceAlarm() {
	/*
	 * Periodic alarm has gone off.
	 * !!! And self is working (WorkClock has alarmed also.)
	 *
	 * Tell clique members:
	 * - sync: now is work time
	 * - period: of WorkFrequency (not of WorkMaintenance clock.)
	 *
	 * Both might have been changed by provisioning
	 * since last time I told clique members.
	 */
	if ( ! _outBoxForWorkSync->tryPut(WorkFrequency::syncPeriodsBetweenWork()) ) {
		RTTLogger::log("Fail mail WorkSync\n");
	}
}

