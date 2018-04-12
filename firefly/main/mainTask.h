
#pragma once

/*
 * Tasks during startup.
 */
class MainTask {
public:
	/*
	 * Invoked from ResetHandler and main.
	 * When power is solar and storage is a capacitor, Vcc and power reserves may be at a bare minimum.
	 * (depends on size of capacitor and voltage monitor thresholds.)
	 * Start LongClock (RTC) and initialize power measuring device.
	 * Then we can sleep mcu and wait for power reserves to increase.
	 * Can't do more, else might brownout.
	 */
	static void onReset();

	/*
	 * Schedule a task that checks power.
	 * If power reserve is much,
	 * then task will schedule onMuchPowerReserve,
	 * else task will reschedule itself to check power again later.
	 */
	static void scheduleCheckPowerReserve();

	/*
	 * Invoked (callback) after power reserves adequate.
	 *
	 * Finish initializing app.
	 * Then sleepsync will schedule initial sync period task
	 * and continuing sequence of tasks will execute.
	 */
	static void onMuchPowerReserve();
};
