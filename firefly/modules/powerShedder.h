
#pragma once

#include <nRF5x.h>	// PowerManager
#include "worker.h"

/*
 * Knows how to shed power
 * to prevent Vcc > Vmax == 3.6V
 */

class PowerShedder {
public:
	static void init(PowerManager* aPowerManager, Worker* aWorker);
	static void shedPowerUntilVccLessThanVmax();
};
