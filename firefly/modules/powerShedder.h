
#pragma once

#include <syncAgent/modules/syncPowerManager.h>
#include "worker.h"

/*
 * Knows how to shed power
 * to prevent Vcc > Vmax == 3.6V
 */

class PowerShedder {
public:
	static void init(SyncPowerManager* aSyncPowerManager, Worker* aWorker);
	static void shedPowerUntilVccLessThanVmax();
};
