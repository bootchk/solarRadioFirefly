
#include <cassert>

#include "powerShedder.h"


namespace {
	SyncPowerManager* syncPowerManager;
	Worker* worker;
}


void PowerShedder::init(SyncPowerManager* aSyncPowerManager, Worker* aWorker) {
	syncPowerManager = aSyncPowerManager;
	worker = aWorker;
}


/*
 * Work locally until power is not excess.
 *
 * We must reduce voltage to keep from exceeding Vmax of chip.
 */
// TODO should start another timer to check again, no while loop here.
void PowerShedder::shedPowerUntilVccLessThanVmax(){
	/*
	 * A call when assert(powerManager->isPowerExcess()) is false is superfluous.
	 * However, Vcc may have dropped after the decision to call
	 */

	do {
		// Here we shed by flashing LED.  Better to use some other device?
		worker->workAmount(200);
		// assert LED on and timer 2 started

		// TODO no sleep here, this is at sync point, should return to sync loop
		// any interrupt may wake a sync sleep, but it will continue
		MCU::sleep();
	}
	while (syncPowerManager->isPowerExcess());

	// Power could have climbed above Vmax since the loop ended.
}
