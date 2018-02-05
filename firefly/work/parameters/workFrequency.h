
#pragma once

#include <inttypes.h>

class WorkFrequency {
public:
	/*
	 * How many sync periods between work.
	 *
	 * If too large, work will exhaust power and syncing could suffer.
	 *
	 * When work is randomized: average periods between work.
	 * When work is regular: periods between work.
	 *
	 * E.G. for 2.1 second period, 3 is once every 6 seconds
	 */

	static const unsigned int syncPeriodsBetweenWork();

	static void setSyncPeriodsBetweenWork(uint32_t periods);
};
