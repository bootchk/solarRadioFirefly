
#pragma once


#include <nRF5x.h>
#include <syncAgent/modules/syncPowerManager.h>

/*
 * Knows:
 * - work amount manages system voltage (loosely called power.)
 * - work is group work
 *
 * This has two goals:
 * 1. use work to avoid excess voltage on mcu
 * 2. do as much work as we can without putting mcu into brownout
 *
 * Goal one could be accomplished with say a zener diode.
 * Goal one could be accomplished without doing work (flashing an LED), say by shorting a resistor on a GPIO.
 *
 * Goal two is application specific where work is analog (flashing an LED)
 * and we want it to be enough to visible.
 * If work is digital, say sending sensor data, the goal doesn't make much sense.
 */

class WorkSupervisor {
public:
	static void init(
			Mailbox*, 	        // Sends work out
			Mailbox* 	        // Sends work local
			);
	// also uses SyncPowerManager and LEDService to consume power

	/*
	 * Invoke some strategy.
	 */
	static void manageVoltageAndWork();

	/*
	 * Work on this unit if power allows.
	 */
	static void tryWorkLocally();

	static void queueLocalWork(WorkPayload work);
};
