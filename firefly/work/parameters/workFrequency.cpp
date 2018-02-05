
#include "workFrequency.h"

#include <syncAgent/logging/logger.h>

namespace {
	unsigned int _syncPeriodsBetweenWork = 4;
}

const unsigned int WorkFrequency::syncPeriodsBetweenWork() {
	return _syncPeriodsBetweenWork;
}

void WorkFrequency::setSyncPeriodsBetweenWork(uint32_t periods) {
	Logger::log("Set work frequency\n");
	Logger::logInt(periods);

	if (periods > 0)
		_syncPeriodsBetweenWork = periods;
	else
		;	// Zero means infinitely often, not allowed.
}
