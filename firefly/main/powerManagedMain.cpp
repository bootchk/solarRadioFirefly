/*
 * Main is application.
 * But it relinquishes (inversion) control to SleepSync library, which calls back.
 *
 * This is a more advanced test harness for SleepSync,
 * where application and SleepSync are also cognizant of power.
 *
 * The differences are:
 * we flash LED to save power instead of leaving it on
 */



#include <cassert>

#include "mainTask.h"

#include <sleepers/syncPowerSleeper.h>
#include <syncAgent/syncAgent.h>

// Unit testing
//#include <logging/logger.h>
//#include <random.h>



// OLD DESIGN, where we sleep.  NEW design uses tasks.


__attribute__ ((noreturn)) void powerManagedMain();


// Objects from radioSoC i.e. platform abstraction
// devices
// Pure class  widely used by WorkSupervisor, SleepSyncAgent
// Nvic , DCDCPowerSupply HfCrystalClock



/*
 * SleepSync init is:
 * Stitch together objects that use each other.  Order is important.
 *
 * nvic, powerSupply, hfClock not need init

 * sleepSyncAgent owns and inits Sleeper instance which requires longClockTimer
 * which itself need nvic.
 *
 * sleepSyncAgent:
 * - uses radio instance which uses other devices
 * - uses LongClockTimer for nowTime
 * - communicates using mailBox
 */





void sleepUntilSyncPower() {
		SyncAgent::initSleepers();

		//Phase::set(PhaseEnum::sleepAfterBoot);
		/*
		 * Sleep until power builds up: boot is at a voltage (a low power reserve)
		 * and there is a small hysteresis on the voltage monitor IC that boots us.
		 */
		SyncPowerSleeper::sleepUntilSyncPower();
}




__attribute__ ((noreturn))
void powerManagedMain() {
	// assert embedded system startup is done and calls main.
	// assert platform initialized radio


	MainTask::onReset();

	sleepUntilSyncPower();
	// Assert power sufficient for SleepSync

	MainTask::onMuchPowerReserve();

	// Debugging stopped counter
	// LongClock::waitOneTick();

	// If init consumes significant power, optionally wait for it to recover
	///SyncPowerSleeper::sleepUntilSyncPower();


	/*
	Unit testing harness
	 while(true) {
		Logger::log((unsigned char) randBool());
		LongClock::waitOneTick();
	}
	*/

	// never returns
	SyncAgent::loop();
}


