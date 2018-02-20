
#pragma once

/*
 * A clock that alarms when it is time
 * for master to sync clique members WorkClocks
 * by sending WorkSync.
 *
 * Only a master ticks this clock,
 * and sends WorkSync.
 *
 * Used by WorkSyncMaintainer.
 * Alarm is hard wired to WorkSyncMaintainer.
 */
class WorkSyncMaintenanceClock {
public:
	static void tick();
};
