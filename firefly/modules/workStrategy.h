
#pragma once

class WorkStrategy {
private:
	/*
	 * How many sync periods between work.
	 *
	 * If too large, work will exhaust power and syncing could suffer.
	 */

	// work is randomized: average periods between work.
	static const unsigned int InverseProbabilityToWork = 3;

	// work is regular: periods between work.
	static const unsigned int SyncPeriodsBetweenWork = 3;

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
	 * Work locally regardless whether master or enough power.
	 * For testing capacity of storage.
	 */
	static void doRegularLocalWork();
};
