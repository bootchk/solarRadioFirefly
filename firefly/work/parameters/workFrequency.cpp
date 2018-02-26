
#include "workFrequency.h"

// lib nRF5x
#include <drivers/mcu.h>


#include <logging/logger.h>

// e.g. 8 seconds when syncs are every 2 seconds
#define DEFAULT_SYNCS_BETWEEN_WORK 4

namespace {
	unsigned int _syncPeriodsBetweenWork = DEFAULT_SYNCS_BETWEEN_WORK;
}

unsigned int WorkFrequency::syncPeriodsBetweenWork() {
	return _syncPeriodsBetweenWork;
}

void WorkFrequency::setSyncPeriodsBetweenWork(uint32_t periods) {
	Logger::log("Set work frequency\n");
	Logger::logInt(periods);

	if (periods > 0)
		_syncPeriodsBetweenWork = periods;
	else
		// Zero means infinitely often, not allowed.
		MCU::breakIntoDebuggerOrHardfault();
}


void WorkFrequency::setSyncPeriodsBetweenWorkCoded(uint32_t workFreqCode ) {

	unsigned int periods = DEFAULT_SYNCS_BETWEEN_WORK;

	// Must correspond to encoding used by provisioner client app
	switch(workFreqCode) {
	case 0:  periods = 1; break;	// 2 seconds
	case 1:  periods = 2; break;	// 4 seconds
	case 2:  periods = 4; break;	// 8 seconds
	}
	setSyncPeriodsBetweenWork(periods);
}
