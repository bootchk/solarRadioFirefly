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


// Project has include path to nRF5x library
#include <nRF5x.h>

// Project has include path to sleepSyncAgent project
#include <sleepSyncAgent.h>

// project local
#include "modules/groupWork.h"
#include "modules/workSupervisor.h"

#include "fireflyConfig.h"


void onWorkMsg(WorkPayload work);
void onSyncPoint();
void initObjects();
__attribute__ ((noreturn)) void powerManagedMain();


namespace {

SleepSyncAgent sleepSyncAgent;

// Objects from nRF5x library i.e. platform

// devices
PowerManager powerManager;
Radio radio;
LongClockTimer longClockTimer;
Nvic nvic;
PowerSupply powerSupply;
HfCrystalClock hfClock;
LEDService ledService;
MCU mcu;

// Not devices
Mailbox myOutMailbox;
Mailbox myInMailbox;


// My objects
WorkSupervisor workSupervisor;





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


// callback from LongClockTimer
void onTimerExpire() { return; }

/*
 * Sleep, waking periodically, until power is sufficient for radio.
 */
void sleepUntilRadioPower() {
	while (!powerManager.isPowerForRadio()){
		// Use Timer 0, later used by SleepSync
		longClockTimer.startTimer((TimerIndex) 0, 40000, onTimerExpire);	// 40k == 1.2seconds
		assertUltraLowPower();
		mcu.sleep();
	}
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
 * SleepSyncAgent received and queued a work msg.
 * I.E. other units are working, in sync, so self should work if it can.
 *
 * This method is realtime constrained: it is called in middle of a sync slot.
 *
 * FUTURE schedule low priority work thread/task to do work.
 */
void onWorkMsg(WorkPayload work) {
	// Queue work to be done later (at next sync point)
	myInMailbox.put(work);
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
 * Here all work is done at they sync point since goal is synchronized work (firefly flashing.)
 * Work itself need not be synchronized, only the SyncAgents.
 * I.E. we could do work at some other time in the sync period,
 * or accumulate work, etc.
 */
void onSyncPoint() {
	// If voltage is excess, this may not return immediately,
	// and disturbs sync
	workSupervisor.manageVoltageByWork();

	/*
	 * Might have queued work out:
	 * - randomly, if power is good but not excess
	 * - if power is excess
	 *
	 * Might have done local work just to shed power.
	 * In that case, we flashed now, and will try to flash again
	 * when our outgoing work bounces back at next sync point.
	 * If the now flash is still in progress at next sync point,
	 * the try will fail (just one long flash.) ???
	 */

	if (myInMailbox.isMail()) {
		// Do work bounced (that I sent) or received in previous sync slot
		WorkPayload work = myInMailbox.fetch();
		workSupervisor.tryWorkInIsolation();
	}
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

	radio.init(
			&nvic,
			&powerSupply,
			&hfClock);
	// sleepSyncAgent will connect msgReceivedCallback and handle all messages

	// !!! HfCrystalClock uses nvic
	hfClock.init(&nvic);
	assert(! hfClock.isRunning());	// xtal not running

	workSupervisor.init(&myOutMailbox, &myInMailbox, &longClockTimer, &ledService, &powerManager);

	sleepSyncAgent.init(&radio, &myOutMailbox, &longClockTimer, onWorkMsg, onSyncPoint);
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

	longClockTimer.init(&nvic);	// sleep needs timer
	// assert counter is perpetually running
	// assert counter interrupt enabled for overflow
	// assert RTC0_IRQ is enabled (for Counter overflow and any Timers)

	sleepUntilRadioPower();

	// Should be a small reserve of power
	initObjects();

	sleepSyncAgent.loopOnEvents(&powerManager);	// never returns
}


