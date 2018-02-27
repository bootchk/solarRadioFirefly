
#pragma once

typedef void (*AlarmCallback)(void);

/*
 * Software clock whose alarm means "time to work"
 *
 * Ticks derived from syncPeriod clock.
 *
 * Masters and slaves  work on their own clocks,
 * which are kept synchronized.
 */

/*
 * !!! Alarm delegate is hardcoded.
 */


class WorkClock {
public:
	/*
	 * Advance clock, possibly triggering alarm and work.
	 */
	static void tickSyncPeriod();

	/*
	 * Synchronize my clock and set its period.
	 * The given period might be same as current period.
	 *
	 * Called when slave's workClock is kept in sync by messages from master.
	 * Clock is set to zero, i.e. synced to now at time of call.
	 */
	static void syncToNow(unsigned int period);

	/*
	 * Synchronize my clock to a tick in the past.
	 * Does not change period (workCycle.)
	 * Clock is set in range [0, period]
	 *
	 * Called when master is provisioned (at behest of user).
	 * Later, master's clock is propagated to slaves.
	 */
	static void syncToPast(unsigned int elapsedPeriods);

	/*
	 * WorkSyncMaintainer calls this and distributes master period to slaves.
	 */
	static unsigned int getPeriod();


#ifdef NOT_USED

	// Alternative: for now, hard code the calls
	// Register one of two alarm callbacks
	// static void registerAlarmCallback(int callbackIndex, AlarmCallback);


	// For design where mark and period are separate
	/*
	 * Reset clock to start at offset from syncpoint
	 *
	 * Units are fractions of syncperiod
	 * offset is numerator or ratio to 255)
	 */
	static void synchronize(unsigned char offset);

	/*
	 * set periodic alarm
	 *
	 * units are syncPeriods
	 */
	static void setAlarmPeriod(unsigned char alarmPeriod);
#endif
};
