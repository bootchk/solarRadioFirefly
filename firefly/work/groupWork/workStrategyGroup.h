
#pragma once

/*
 * Decides whether and how much to work (locally and/or in unison with clique).
 *
 * Called regularly (every SyncPeriod).
 *
 * Every call might work.
 * Every call might change amount of work (side effect for later work.)
 *
 * Some strategies are more sophisticated and use other objects.
 *
 * This class as an object is just a collection of alternative methods
 * that all do the same thing: decide whether to work.
 */
class WorkStrategyGroup {
private:

	static void doRandomWork();
	static void doRegularWork();

public:
	/*
	 * WorkSupervisor chooses one of these alternatives.
	 */

	/*
	 * Do work to reduce Vcc below Vmax,
	 * and do work regularly otherwise.
	 * Adjusts amount of work to help prevent Vcc above Vmax.
	 * Used when solar cell has Voc greater than radioSoC Vmax.
	 */
	static void manageExcessPowerWithWork();

	/*
	 * If self is Master and exists enough power, send work randomly.
	 */
	static void manageWorkOnlyRandomlyIfPowerAndMaster();

	/*
	 * If self is Master and exists enough power, send work regularly.
	 */
	static void manageWorkOnlyRegularlyIfPowerAndMaster();

	/*
	 * On every call work locally regardless whether master or enough power.
	 * But don't tell others.
	 * For testing capacity of storage.
	 */
	static void doRegularLocalWork();
};
