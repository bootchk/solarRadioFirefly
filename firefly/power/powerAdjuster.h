
#pragma once


/*
 * Knows how to adjust power usage.
 *
 * Chooses from ways:
 * - work
 * - fishing
 * Balances choices.
 */

// TODO conflates energy, voltage, and power

class PowerAdjuster {
public:
	static void setUnconstrainedEnergy();
	static void onExcessVoltage();

	/*
	 * Adjust energy usage
	 */
	static void increaseUsage();
	static void decreaseUsage();
	static void maintainUsage();
	static void setLeastUsage();
};
