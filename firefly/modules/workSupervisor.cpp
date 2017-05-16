
// Config depends on POWER_IS_SOLAR in build config


#include <nRF5x.h>

#include "workSupervisor.h"
#include "worker.h"
#include "groupWork.h"


// dummy workpayload represents version of code
#define WORK_VERSION 34



/*
 * Implementation notes:
 *
 * Self uses PowerManager to adjust work on Worker.
 *
 * Self manages the amount of work on Worker,
 * but GroupWork tells Worker to work.
 */
namespace {

PowerManager* powerManager;

// owns
Worker worker;
GroupWork groupWork;


/*
 * Work locally until power is not excess.
 *
 * We must reduce voltage to keep from exceeding Vmax of chip.
 */
// TODO should start another timer to check again, no while loop here.
void workLocallyToShedPower() {
	do {
		worker.workAmount(200);
		// assert LED on and timer 2 started

		// TODO no sleep here, this is at sync point, should return to sync loop
		// any interrupt may wake a sync sleep, but it will continue
		MCU::sleep();
	}
	while (powerManager->isPowerExcess());
}



/*
 * A very simple approach, not very effective?
 */
// TODO replace this with a state machine that is more effective

void simpleManagePowerWithWork() {

	/* It is possible for power levels to drop so precipitously that
	 * on consecutive calls, the VoltageRange's are not adjacent,
	 * i.e. pass from Excess to Medium or worse.
	 */
	switch (powerManager->getVoltageRange()) {
	case VoltageRange::Excess:
		/* e.g. > 2.7V.
		 * Self MUST work locally to keep voltage from exceeding Vmax
		 * Send to group.
		 */
		worker.increaseAmount();
		groupWork.initiateGroupWork(WORK_VERSION);

		CustomFlash::writeZeroAtIndex(ExcessPowerEventFlagIndex);

		workLocallyToShedPower();
		// assert power is not excess?
		// TODO if change ShedPower, Vcc might still be excess
		break;

	case VoltageRange::High:
		/* e.g. 2.5V - 2.7V
		 * I could work (enough power) but I don't need to work to manage excess power.
		 */
		worker.maintainAmount();

		/*
		 * This may cause local work.
		 * Thus we might do two local works in consecutive calls.
		 * I.E. For Excess, then for High
		 * I.E. For High, then High (when both random results are true.)
		 */
		groupWork.randomlyInitiateGroupWork(WORK_VERSION);
		break;

	case VoltageRange::Medium:
		/* e.g. 2.3-2.5V
		 * Decrease amount of work
		 *
		 * not initiate work.
		 * Others may ask me to work, see elsewhere.
		 */
		worker.decreaseAmount();
		break;

	case VoltageRange::Low:
		/* e.g. 2.1-2.3V
		 * Not enough power to work, and power getting low.
		 * Next work done (if ever) is least amount.
		 */
		worker.setLeastAmount();
		break;

	case VoltageRange::UltraLow:
		/* e.g. <2.1-2.3V
		 * Not enough power to work.
		 * Power nearing brownout.
		 * Next work done (if ever) is least amount.
		 *
		 * It is possible for power levels to drop so precipitously that
		 * we didnt' pass through case above for Low.
		 */
		CustomFlash::writeZeroAtIndex(BrownoutPowerEventFlagIndex);
		worker.setLeastAmount();
		break;
	}
}


/*
 * For testing, when not solar powered.
 * Periodically, when called back by SyncAgent, initiate work.
 */
void doRandomWork() {
	//Default amount of work is enough to perceive, but make it more visible...
	worker.setAmountPerceivable();

	groupWork.randomlyInitiateGroupWork(WORK_VERSION);
}




} // namespace



void WorkSupervisor::init(
		Mailbox* aOutMailbox,
		Mailbox* aInMailbox,
		LongClockTimer* aLCT,
		LEDService* aLEDService,
		PowerManager* aPowerManager)
{
	powerManager = aPowerManager;
	worker.init(aLCT, aLEDService);

	// self doesn't use mailbox, merely passes mailbox to groupWorker
	// self owns worker, but groupWorker also uses it
	groupWork.init(aOutMailbox, aInMailbox);

	// PowerManager needs no init
}



/*
 * Part of the total voltage/power management.
 * This is high side.
 * See also SyncSleep, which also does power managment on the low side.
 *
 * Here, if voltage is climbing, do more work.
 * Note work is asynchronous, more work does not require more time in this routine.
 *
 * This is at syncpoint (but it may be drifted.)
 *
 * This also determines the behaviour of the flashing:
 * - units try to flash in sync
 * - units with excess power may flash when other units can't
 * - when all units have moderate power, some unit may randomly trigger all to flash
 *
 */
void WorkSupervisor::manageVoltageByWork() {

/*
 * Peculiar to my test setup:
 *
 * one unit is a nRF52DK, with regulated, line power, thus there is always enough power
 * and never too much: no need to manage voltage.
 *
 * The other units are nRF51 with unregulated solar power.
 * Depending on the light, the voltage may exceed Vmax 3.6V of the unit,
 * so this algorithm attempts to use power to control voltage.
 *
 * (The solar cells can produce a max of 4.8V, which is not exceedingly dangerous to the chips.)
 */
#ifdef POWER_IS_SOLAR
	simpleManagePowerWithWork();
#else
	doRandomWork();
#endif


}


void WorkSupervisor::tryWorkInIsolation() {
	if (powerManager->isPowerForWork()) {

		// Record that we worked at least once.
		CustomFlash::writeZeroAtIndex(WorkEventFlagIndex);

		log("Do work\n");
		worker.workManagedAmount();
	}
	else {
		// omit work, not enough power
		log("Not enough power to work\n");
	}

}


void WorkSupervisor::queueLocalWork(WorkPayload work) {
	groupWork.queueLocalWork(work);
}

