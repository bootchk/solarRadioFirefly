
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
	 * !!! These methods not called for certain build configurations
	 * where power is unconstrained and voltage is regulated in HW,
	 * e.g. on the dev kit (instead of the solar powered-target)
	 * i.e. where we are not checking for Vcc exceed Vmax
	 */
	/*
	 * Adjust energy usage.
	 * Delegated to a strategy.
	 */
	static void increaseUsage();
	static void decreaseUsage();
	static void maintainUsage();
	static void setLeastUsage();
};
