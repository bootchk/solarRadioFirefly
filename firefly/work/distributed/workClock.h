
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
	 * Synchronize my clock and possibly set its period.
	 *
	 * Called:
	 * - when master is provisioned (at behest of user)
	 * - when slave's workClock is kept in sync by messages from master.
	 */
	static void setSync(unsigned char period);

	/*
	 * WorkSyncMaintainer calls this and distributes master period to slaves.
	 */
	static unsigned char getPeriod();


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
