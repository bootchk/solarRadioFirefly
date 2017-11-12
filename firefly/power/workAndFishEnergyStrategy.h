#pragma once

/*
 * Strategy for adjusting energy use that uses:
 * - Worker
 * - Fishing
 */

class WorkAndFishEnergyStrategy {
public:
	static void increaseUsage();
	static void decreaseUsage();
	static void maintainUsage();
	static void setLeastUsage();
};
