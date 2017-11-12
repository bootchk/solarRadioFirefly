
#pragma once

/*
 * Strategy for adjusting energy use that only uses Worker
 */

class WorkerOnlyEnergyStrategy {
public:
	static void increaseUsage();
	static void decreaseUsage();
	static void maintainUsage();
	static void setLeastUsage();
};
