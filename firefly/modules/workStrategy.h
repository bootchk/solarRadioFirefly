
#pragma once

class WorkStrategy {
private:
	/*
	 * How many sync periods between work.
	 *
	 * If too large, work will exhaust power and syncing could suffer.
	 *
	 * If work is randomized, this is the average periods between work.
	 */
	static const unsigned int InverseProbabilityToWork = 3;

public:
	static void simpleManagePowerWithWork();
	static void doRandomWork();
	static void doRegularWork();

	/*
	 * If self is Master and exists enough power, send work randomly.
	 */
	static void manageWorkOnlyRandomlyIfPowerAndMaster();
};
