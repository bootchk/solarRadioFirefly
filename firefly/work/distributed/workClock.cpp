
#include "workClock.h"

// hardcode observers of alarm
#include "distributedSynchronizedWorker.h"
#include "workSyncMaintainer.h"
#include "../parameters/workFrequency.h"





namespace {
/*
 * Counter is circular, and wraps without harm.
 * But only if period divides evenly into counter size!!!!
 * Alternatively, we could handle wrap differently.
 *
 * It would only wrap if WorkSyncMaintainer never provisions with WorkSync.
 */
unsigned int _counter = 0;


static void onWorkClockAlarm() {

	// Work required in this syncPeriod.  E.g. flash LED
	DistributedSynchronizedWorker::onWorkAlarm();

	/*
	 * tick WorkSyncMaintenance clock with alarms from WorkClock.
	 * I.E. an alarm from one clock ticks another clock.
	 */
	WorkSyncMaintainer::onWorkAlarm();
}


}


void WorkClock::tickSyncPeriod() {
	_counter++;

	// 0 is arbitrary choice, but counter continues to increment
	if ( (_counter % WorkFrequency::syncPeriodsBetweenWork()) == 0) {
		onWorkClockAlarm();
	}
}


void WorkClock::syncToNow(unsigned int period) {
	// All unit's clocks back to zero
	_counter = 0;

	WorkFrequency::setSyncPeriodsBetweenWork(period);
}




/*
 * Note there is unused design to work at a syncPeriodOffset from syncPoint.
 * Here, we always work at a syncPoint.
 * Thus separate clique's work is not synchronized yet.
 */
/*
 * Called at provisioning time.
 * elapsedSyncPeriods is in real (wall) time, with no delay.
 * Convert elapsedSyncPeriods wall time (from button push) into
 * clockAdvance on the current WorkClock.
 * That fixes it against the distributed WorkClock.
 */
unsigned int WorkClock::convertPeriodsElapsedToClockAdvance(unsigned int  elapsedSyncPeriods) {
	return elapsedSyncPeriods % WorkFrequency::syncPeriodsBetweenWork();
}


void WorkClock::syncToPast(unsigned int clockAdvance) {
	/*
	 * A "mark" (a user pushing a button) was pushed at the clockAdvance tick of clock.
	 * Sync the work clock with the mark.
	 *
	 * This may make the next work sooner or later than it would have been.
	 */
	_counter = ( _counter + clockAdvance);
	/*
	 *  !!! Not assert _counter < period.
	 *  See ticking, it is free running counter.
	 */
}


