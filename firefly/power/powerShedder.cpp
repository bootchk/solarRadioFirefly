
#include <cassert>

// sleepSyncAgent
#include <modules/syncPowerManager.h>

#include "powerShedder.h"
#include "../work/worker.h"

// FIXME should not use drivers directly
#include "drivers/mcu.h"


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
	// Requires Worker and PowerShedder init

	do {
		// Here we shed by flashing LED.  Better to use some other device?
		Worker::workAmount(200);
		// assert LED on and timer 2 started

		// TODO no sleep here, this is at sync point, should return to sync loop
		// any interrupt may wake a sync sleep, but it will continue
		MCU::sleepUntilEvent();
	}
	while (SyncPowerManager::isPowerExcess());

	// Power could have climbed above Vmax since the loop ended.
}
