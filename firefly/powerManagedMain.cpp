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

// Config depends on BOARD_xxx defined in build configuration

#include <cassert>

// Uses radioSoC and SleepSync libraries.  Projects build configs have path to these
#include <radioSoC.h>
#include <syncAgent/syncAgent.h>
#include <syncAgent/state/phase.h>
#include <syncAgent/sleepers/syncPowerSleeper.h>
#include <syncAgent/modules/syncPowerManager.h>

// project local
#include "modules/groupWork.h"

// Uses pure classes
#include "modules/workSupervisor.h"
/*
 * Uses pure classes:
 *
 * SyncAgent
 * SyncPowerManager
 *
 * LongClock
 * Timer
 */



void onWorkMsg(WorkPayload work);
void onSyncPoint();
void initObjects();
__attribute__ ((noreturn)) void powerManagedMain();




namespace {

// Objects from nRF5x library i.e. platform

// devices
// Pure class  widely used by WorkSupervisor, SleepSyncAgent

// Nvic , DCDCPowerSupply HfCrystalClock



// Not devices
Mailbox myOutMailbox;
Mailbox myInMailbox;



void initLEDs() {

#if BOARD_NRF52DK
	// nRF52DK board (from pca10040.h)
	LEDService::init(4, true, 17, 18, 19, 20);
#elif BOARD_REDBEAR_NANO || BOARD_WAVESHARE
	LEDService::init(1, McuSinks, 19, 0, 0, 0);
#elif BOARD_UBLOX_NINA_SOURCE_LED_REV1
	// uBlox, one LED, source, UBlox pin 16, NRF52 P0.28
	LEDService::init(1, false, 28, 0, 0, 0);
#elif BOARD_UBLOX_NINA_SINK_LED
	// July 2017 my test board, this doesn't work.  Must be a board problem.
	LEDService::init(1, McuSinks, 29, 0, 0, 0);
#elif BOARD_UBLOX_NINA_SINK_LED_REV2
	// uBlox, one LED, sunk, my footprint pin 13, UBlox pin 29, NRF52 P0.10
	LEDService::init(1, true, 8, 0, 0, 0);
#elif BOARD_WAVESHARE2_REV1
	LEDService::init(1, true, 30, 0, 0, 0);
#elif BOARD_WAVESHARE2_REV2
	LEDService::init(1, true, 8, 0, 0, 0);

#else
#error "No board defined"
#endif
assert(LEDService::wasInit());
}

} // namespace




/*
 * The app understands power for work.
 * SleepSyncAgent further understands power for radio.
 *
 * SleepSyncAgent will sync-keep, or sync-maintain depending on power.
 *
 * The app sends work OUT only if self has enough power.
 * SleepSyncAgent will convey work IN if it hears it,
 * but app will not act on it if not enough power..
 */


/*
 * SleepSyncAgent received a work msg.
 * I.E. other units are working, in sync, so self should work if it can.
 *
 * This method is realtime constrained: it is called in middle of a sync slot.
 *
 * FUTURE schedule low priority work thread/task to do work.
 */
void onWorkMsg(WorkPayload work) {
	// Queue work to be done later (at next sync point)
	WorkSupervisor::queueLocalWork(work);
}



/*
 * Managing voltage requires periodic checking, this is a convenient place.
 *
 * This may initiate group work (working locally and as a group.)
 *
 * Must be short duration, else interferes with sync.
 * FUTURE make this a thread and yield to higher priority sleep sync thread
 */
/*
 * Here we start work at the SyncPoint since goal is synchronized work (firefly flashing.)
 * Work itself need not be synchronized, only the SyncAgents.
 * I.E. we could do work at some other time in the sync period, or accumulate work, etc.
 *
 * !!! If doing work (as opposed to just starting it) takes a long time,
 * it should be done in the work slot.
 * Doing work at the SyncPoint will disrupt sync if it takes too long.
 */
void onSyncPoint() {

	if (myInMailbox.isMail()) {
		// Do work bounced (that I sent) or received in previous sync slot
		// For now, ignore work specifics
		(void) myInMailbox.fetch();
		WorkSupervisor::tryWorkLocally();
	}
	/*
	 * Work from others might have depleted my power.
	 * But I might have enough power to initiate work again.
	 */

	// A choice is made inside, e.g. work regularly or work only if power
	// If voltage is excess, this may not return immediately, and disturbs sync
	WorkSupervisor::manageVoltageAndWork();

	/*
	 * Might have queued work out and in:
	 * - randomly, if power is good but not excess
	 * - if power is excess
	 *
	 * Might have done local work just to shed power.
	 * In that case, we flashed now, and will try to flash again
	 * when our outgoing work bounces back at next sync point.
	 * If the now flash is still in progress at next sync point,
	 * the try will fail (just one long flash.) ???
	 */
}

/*
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
void initObjects() {

	// WorkSupervisor uses LED's, pure classes LEDFlasher which uses LEDService and Timer
	initLEDs();

	WorkSupervisor::init(&myOutMailbox, &myInMailbox);

	// Initialize SyncAgent (Ensemble) and connect it to app
	SyncAgent::initSyncObjects(&myOutMailbox, onWorkMsg, onSyncPoint);
}



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

	clearResetReason();

	/*
	 * assert interrupts globally enabled i.e. PRIMASK
	 * Starting clocks will enable IRQ and interrupt on event
	 */

	/* Start LFClock and LongClock, sleeping until.
	 * Takes 0.6mSec for LFRC to be running, .25seconds for LFXO
	 *
	 * Formerly just LongClock::start(); but didn't seem to insure Timer would work.
	 */
	ClockFacilitator::startLongClockWithSleepUntilRunning();

	// Now we can use Timer

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

	initObjects();
	// sleepSyncAgent prepared to loop

	// Artifact of debugging to detect stopped counter
	LongClock::waitOneTick();

	SyncPowerSleeper::sleepUntilSyncPower();
	// sleepSyncAgent has enough power to start

	SyncAgent::loop();	// never returns
}


