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
 * Uses radioSoC and SleepSync libraries.
 * Project build configs have path to these
 */
#include <syncAgent/syncAgent.h>
#include <syncAgent/state/phase.h>
#include <sleepers/syncPowerSleeper.h>
#include <modules/syncPowerManager.h>




#include "other/boardManager.h"

#include "work/groupWork.h"
#include "work/workSupervisor.h"
#include "work/parameters/workTime.h"
#include "work/parameters/workFrequency.h"
#include "power/powerAdjuster.h"

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
void doReceivedWork();
__attribute__ ((noreturn)) void powerManagedMain();




namespace {

// Objects from radioSoC i.e. platform abstraction

// devices
// Pure class  widely used by WorkSupervisor, SleepSyncAgent

// Nvic , DCDCPowerSupply HfCrystalClock

// Not devices
Mailbox myOutMailbox;
Mailbox myInMailbox;

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
 * Here we start work at the SyncPoint since goal is synchronized work (firefly flashing.)
 * Work itself need not be synchronized, only the SyncAgents.
 * I.E. we could do work at some other time in the sync period, or accumulate work, etc.
 *
 * Work requires no cpu (uses timer), i.e. is a separate task.
 * But starting work task takes time.
 * !!! If starting work takes a long time,
 * work should be started elsewhere (not at the sync point, say in a work slot, or after sync slot.)
 * Otherwise, starting work may disrupt sync if it takes too long.
 */
void doReceivedWork() {
	if (myInMailbox.isMail()) {
		// Do work bounced (that I sent) or received in previous sync slot
		// For now, ignore work specifics
		(void) myInMailbox.fetch();
		WorkSupervisor::tryWorkLocally();
	}
}

/*
 * Managing voltage requires periodic checking, this is a convenient place.
 *
 * This may initiate group work (working locally and as a group.)
 *
 * Must be short duration, else interferes with sync.
 * FUTURE make this a thread and yield to higher priority sleep sync thread
 */

void onSyncPoint() {

	doReceivedWork();
	/*
	 * Received work from others might have depleted my power.
	 * (But we just started a work task, it probably didn't use much power yet.)
	 * But I might have enough power to initiate work again.
	 */

	/*
	 * WorkSupervisor chooses a strategy:, e.g. work regularly or work only if power
	 * One strategy: if voltage is excess, this may not return immediately, and disturbs sync
	 */
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
	BoardManager::initLEDs();

	WorkSupervisor::init(&myOutMailbox, &myInMailbox);

	// Initialize SyncAgent (Ensemble) and connect it to app
	SyncAgent::initSyncObjects(&myOutMailbox, onWorkMsg, onSyncPoint);

	// Connect provisioning
	SyncAgent::subscribeProvisioning(0, WorkTime::setPeriodTimeToWork);
	SyncAgent::subscribeProvisioning(1, WorkFrequency::setSyncPeriodsBetweenWork);
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
	 * Formerly just ClockFacilitator::startLongClockWithSleepUntilRunning();
	 * but that requires access to PowerClock, which is blocked by SD.
	 *
	 * Current code starts long clock (lfclock and rtc)
	 * The timers will eventually work.
	 * The first timeout is over long or inaccurate, until lfclock is stable.
	 */

	ClockFacilitator::startLongClockNoWaitUntilRunning();

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

	initObjects();
	// sleepSyncAgent prepared to loop

#ifndef POWER_IS_SOLAR
	PowerAdjuster::setUnconstrainedEnergy();
#endif

	// Artifact of debugging to detect stopped counter
	LongClock::waitOneTick();

	SyncPowerSleeper::sleepUntilSyncPower();
	// sleepSyncAgent has enough power to start

	// never returns
	SyncAgent::loop();
}


