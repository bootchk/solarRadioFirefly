
#pragma once

class WorkStrategy {
private:
	static const unsigned int InverseProbabilityToWork = 10;

public:
	static void simpleManagePowerWithWork();
	static void doRandomWork();

	/*
	 * If self is Master and exists enough power, send work randomly.
	 */
	static void manageWorkOnlyRandomlyIfPowerAndMaster();
};
