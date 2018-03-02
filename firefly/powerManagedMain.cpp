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

/*
 * Uses SleepSync libraries.
 * (which requires radioSoC lib)
 * Project build configs have path to these
 */
#include <sleepers/syncPowerSleeper.h>
#include <modules/syncPowerManager.h>
// TODO eliminate this
#include <syncAgentImp/state/phase.h>

#include <syncAgent/syncAgent.h>

#include "workFacilitator.h"

#include "power/powerAdjuster.h"

// Unit testing
#include <logging/logger.h>
#include <random.h>


/*
 * Uses pure classes:
 *
 * SyncAgent
 * SyncPowerManager
 * LongClock
 * Timer
 */


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




__attribute__ ((noreturn))
void powerManagedMain() {
	// assert embedded system startup is done and calls main.
	// assert platform initialized radio

	/*
	 * Compile time test to ensure that macro __FPU_USED is not set.
	 * If it were set, then system_nrf52.c enables FPU hw,
	 * consuming power that
	 */
	#ifdef __FPU_USED
	#error "FPU used: ensure that FPU exceptions do not prevent sleep."
	#endif

#if NRF52
	// This is not crucial:  MCU::enableInstructionCache();
#endif

	/*
	 * POR clears reset reasons.
	 * But we are often debugging, so clear this register now to catch subsequent reset reasons.
	 */
	clearResetReason();


	/*
	 * assert interrupts globally enabled i.e. PRIMASK
	 * Starting clocks will enable IRQ and interrupt on event
	 */

	/* Start LFClock and LongClock, sleeping until.
	 * Takes 0.6mSec for LFRC to be running, .25seconds for LFXO
	 *
	 * Formerly just LongClock::start(); but didn't seem to insure Timer would work.
	 *
	 * Current code starts long clock (lfclock and rtc)
	 * The timers will eventually work.
	 * The first timeout is over long or inaccurate, until lfclock is stable.
	 */

#ifdef SOFTDEVICE_PRESENT
	// requires libNRFDrivers, to use SD compatible modules from SDK
	ClockFacilitator::startLongClockNoWaitUntilRunning();
#else
	// requires access to PowerClock, which is blocked by SD.
	ClockFacilitator::startLongClockWithSleepUntilRunning();
#endif

	// We can use Timer even though it is not accurate

	// SyncPowerSleeper needs these objects before sleepUntilSyncPower()
	SyncPowerManager::init();
	SyncAgent::initSleepers();

	Phase::set(PhaseEnum::sleepAfterBoot);
	/*
	 * Sleep until power builds up: boot is at a voltage (a low power reserve)
	 * and there is a small hysteresis on the voltage monitor IC that boots us.
	 */
	SyncPowerSleeper::sleepUntilSyncPower();

	Phase::set(PhaseEnum::firstSyncPower);

	WorkFacilitator::init();

	// Initialize SyncAgent (Ensemble) and connect it to app
	SyncAgent::initSyncObjects(
			WorkFacilitator::outMailbox(),
			WorkFacilitator::onWorkSyncMsg,
			WorkFacilitator::onSyncPoint);

	// sleepSyncAgent prepared to loop

#ifndef POWER_IS_SOLAR
	PowerAdjuster::setUnconstrainedEnergy();
#endif

	// Artifact of debugging to detect stopped counter
	LongClock::waitOneTick();

	SyncPowerSleeper::sleepUntilSyncPower();
	// sleepSyncAgent has enough power to start

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


