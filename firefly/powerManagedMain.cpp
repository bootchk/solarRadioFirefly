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

// Uses nRF5x and SleepSync libraries.  Projects build configs have path to these
#include <nRF5x.h>
#include <syncAgent/syncAgent.h>

// project local
#include "modules/groupWork.h"

// Uses pure classes
#include "modules/workSupervisor.h"
/*
 * Uses pure classes:
 * sleepSyncAgent::SyncPowerManager
 */



void onWorkMsg(WorkPayload work);
void onSyncPoint();
void initObjects();
__attribute__ ((noreturn)) void powerManagedMain();




namespace {

// Objects from sleepSyncAgent library

SyncAgent sleepSyncAgent;

// Objects from nRF5x library i.e. platform

// devices
// Pure class LongClockTimer widely used by WorkSupervisor, SleepSyncAgent

Nvic nvic;
DCDCPowerSupply dcdcPowerSupply;
HfCrystalClock hfClock;
LEDService ledService;
MCU mcu;


// Not devices
Mailbox myOutMailbox;
Mailbox myInMailbox;



void initLEDs() {

#if BOARD_NRF52DK
	// nRF52DK board (from pca10040.h)
	ledService.init(4, McuSinks, 17, 18, 19, 20);
#elif BOARD_REDBEAR_NANO || BOARD_WAVESHARE
	ledService.init(1, McuSinks, 19, 0, 0, 0);
#elif BOARD_UBLOX_NINA_SOURCE_LED
	ledService.init(1, McuSources, 28, 0, 0, 0);
#elif BOARD_UBLOX_NINA_SINK_LED
	// uBlox, one LED, source, P0.28
	ledService.init(1, McuSinks, 29, 0, 0, 0);
#elif BOARD_WAVESHARE2
	ledService.init(1, McuSinks, 30, 0, 0, 0);
#else
#error "No board defined"
#endif
assert(ledService.wasInit());
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

	initLEDs();

	Radio::init(
			&nvic,
			&dcdcPowerSupply,
			&hfClock);
	// sleepSyncAgent will connect msgReceivedCallback and handle all messages

	// !!! HfCrystalClock uses nvic
	hfClock.init(&nvic);
	assert(! hfClock.isRunning());	// xtal not running

	WorkSupervisor::init(&myOutMailbox, &myInMailbox, &ledService);

	sleepSyncAgent.initSyncObjects(&myOutMailbox, onWorkMsg, onSyncPoint);
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
	MCU::enableInstructionCache();
#endif

	clearResetReason();

	// assert interrupts globally enabled i.e. PRIMASK

	// SyncPowerSleeper needs these objects before sleepUntilSyncPower()
	SyncPowerManager::init();
	LongClockTimer::start();
	// Started but may not be running.  Can still sleep, just won't be accurate?
	// Takes 0.6mSec to start


	sleepSyncAgent.initSleepers();

	/*
	 * Sleep until power builds up: boot is at a voltage (a low power reserve)
	 * and there is a small hysteresis on the voltage monitor IC that boots us.
	 */
	sleepSyncAgent.sleepUntilSyncPower();

	initObjects();
	// sleepSyncAgent prepared to loop

	sleepSyncAgent.sleepUntilSyncPower();
	// sleepSyncAgent has enough power to start

	sleepSyncAgent.loop();	// never returns
}


