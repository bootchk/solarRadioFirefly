

#include  "workFacilitator.h"

#include "other/boardManager.h"

#include "work/workSupervisor.h"

#include "work/distributed/workClock.h"
#include "work/parameters/workFrequency.h"

#include <provisioning/workControlProxy.h>
#ifdef SOFTDEVICE_PRESENT
#include <provisioning/workProvisioner.h>
#endif


namespace {

// WorkSync out to other units
Mailbox myOutMailbox;

/*
 * In some designs, work from other units.
 * In other designs, used internally to queue onWorkSyncMsg() for later processing.
 */
Mailbox myInMailbox;


#ifdef WORK_ON_DEMAND
an old design
/*
 * Here we start work at the SyncPoint since goal is synchronized work (firefly flashing.)
 * Work itself need not be synchronized, only the SyncAgents.
 * I.E. we could do work at some other time in the sync period, or accumulate work, etc.
 *
 * Work requires no cpu (uses timer), i.e. is a separate task.
 * But starting work task takes time.
 * !!! If starting work takes a long time,
 * work should be started elsewhere (not at the sync point, say in a work slot, or after sync slot.)
 * Otherwise, starting work may disrupt sync if it takes too long.
 */
/*
 * Work might have deplete my power.
 * (But we just started a work task, it probably didn't use much power yet.)
 * But I might have enough power to initiate work again.
 */
void doReceivedWork() {
	if (myInMailbox.isMail()) {
		// Do work bounced (that I sent) or received in previous sync slot
		// For now, ignore work specifics
		(void) myInMailbox.fetch();
		WorkSupervisor::tryWorkLocally();
	}
}
#endif

} // namespace



void WorkFacilitator::init() {

	// WorkSupervisor uses LED's, pure classes LEDFlasher which uses LEDService and Timer
	BoardManager::initLEDs();

	WorkSupervisor::init(&myOutMailbox, &myInMailbox);

	// Connect provisioning
	WorkControlProxy::setWorkTimeControlCallback(WorkClock::syncToPast);
	WorkControlProxy::setWorkCycleControlCallback(WorkFrequency::setSyncPeriodsBetweenWorkCoded);

#ifdef SOFTDEVICE_PRESENT
	WorkProvisioner::setConverterFunc(WorkClock::convertPeriodsElapsedToClockAdvance);
#endif
}


Mailbox* WorkFacilitator::outMailbox() { return &myOutMailbox; }


/*
 * The app understands power for work.
 * SleepSyncAgent further understands power for radio.
 *
 * SleepSyncAgent will sync-keep, or sync-maintain depending on power.
 *
 * The app sends work OUT only if self has enough power.
 * SleepSyncAgent will convey work IN if it hears it,
 * but app will not act on it if not enough power..
 */


/*
 * TODO old comments
 * I.E. other units are working, in sync, so self should work if it can.
 *
 * FUTURE do work later
 */
void WorkFacilitator::onWorkSyncMsg(MailContents work) {
	// Delegate
	WorkSupervisor::onWorkSyncMsg(work);
}



/*
 * Managing voltage requires periodic checking, this is a convenient place.
 *
 * This may initiate group work (working locally and as a group.)
 *
 * Must be short duration, else interferes with sync.
 * FUTURE make this a thread and yield to higher priority sleep sync thread
 */

void WorkFacilitator::onPreSyncSlot() {

	// In an old design:  doReceivedWork();
	/*
	 * WorkSupervisor chooses a strategy:, e.g. work regularly or work only if power
	 * One strategy: if voltage is excess, this may not return immediately, and disturbs sync
	 */
	WorkSupervisor::manageVoltageAndWork();

	/*
	 * Might have queued work out and in:
	 * - randomly, if power is good but not excess
	 * - if power is excess
	 *
	 * Might have done local work just to shed power.
	 * In that case, we flashed now, and will try to flash again
	 * when our outgoing work bounces back at next sync point.
	 * If the now flash is still in progress at next sync point,
	 * the try will fail (just one long flash.) ???
	 */
}

