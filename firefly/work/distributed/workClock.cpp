
#include "workClock.h"

// hardcode observers of alarm
#include "distributedSynchronizedWorker.h"
#include "workSyncMaintainer.h"





namespace {
/*
 * Counter is circular, and wraps without harm.
 * But only if period divides evenly into counter size!!!!
 * Alternatively, we could handle wrap differently.
 *
 * It would only wrap if WorkSyncMaintainer never provisions with WorkSync.
 */
unsigned int _counter = 0;
unsigned int _period = 4; // default.  Must be a power of two.


static void onTickWorkClock() {
	DistributedSynchronizedWorker::onWorkAlarm();
	WorkSyncMaintainer::onWorkAlarm();
}


}


void WorkClock::tickSyncPeriod() {
	_counter++;

	// 0 is arbitrary choice
	if ( (_counter % _period) == 0) {
		onTickWorkClock();
	}
}


void WorkClock::syncToNow(unsigned int period) {
	// All unit's clocks back to zero
	_counter = 0;
	_period = period;
}


void WorkClock::syncToPast(unsigned int elapsedSyncPeriods) {
	_counter = elapsedSyncPeriods % getPeriod();
}



unsigned int WorkClock::getPeriod() {
	return _period;
}

