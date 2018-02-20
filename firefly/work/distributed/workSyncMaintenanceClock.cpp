
#include "workSyncMaintenanceClock.h"

#include "workSyncMaintainer.h"



namespace {

/*
 * Counter runs free, wraps without harm
 * Period does not need to be power of two.
 * When it wraps, we might get an extra alarm,
 * but it is in sync with the WorkClock.
 */
unsigned int _counter = 0;

unsigned char _workMaintenancePeriod = 5;

} // namespace



void WorkSyncMaintenanceClock::tick() {
	_counter++;

	// 0 is arbitrary choice
	if ( (_counter % _workMaintenancePeriod) == 0) {
		WorkSyncMaintainer::onWorkSyncMaintenanceAlarm();
	}
}
