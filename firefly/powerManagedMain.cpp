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


// Project has include path to nRF5x library
#include <nRF5x.h>

// Project has include path to sleepSyncAgent project
#include <sleepSyncAgent.h>

// project local
#include "modules/groupWork.h"
#include "modules/workSupervisor.h"


namespace {

SleepSyncAgent sleepSyncAgent;

// Objects from nRF5x library i.e. platform
Radio radio;
Mailbox myOutMailbox;
LongClockTimer longClockTimer;
Nvic nvic;

PowerSupply powerSupply;
HfCrystalClock hfClock;

WorkSupervisor workSupervisor;
LEDService ledService;

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
 * This method is realtime constrained.
 *
 * FUTURE schedule low priority work thread/task to do work.
 */
void onWorkMsg(WorkPayload work) {
	(void) work;	// Unused

	workSupervisor.tryWorkInIsolation();
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
	workSupervisor.manageVoltageByWork();
}




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

	/*
	 * Stitch things together: objects use each other.  Order is important.

	 * sleepSyncAgent owns and inits Sleeper instance which requires longClockTimer
	 * which itself need nvic.
	 *
	 * sleepSyncAgent:
	 * - uses radio instance which uses other devices
	 * - uses LongClockTimer for nowTime
	 * - communicates using mailBox
	 */
	// nvic, powerSupply, hfClock not need init
	longClockTimer.init(&nvic);

	// On this board, one LED, isSunk, P0.29
	ledService.init(1, true, 29, 0, 0, 0);

	radio.init(
			&nvic,
			&powerSupply,
			&hfClock);
	// sleepSyncAgent will connect msgReceivedCallback and handle all messages

	// !!! HfCrystalClock uses nvic
	hfClock.init(&nvic);

	workSupervisor.init(&myOutMailbox, &longClockTimer, &ledService);

	sleepSyncAgent.init(&radio, &myOutMailbox, &longClockTimer, onWorkMsg, onSyncPoint);
	sleepSyncAgent.loopOnEvents();	// never returns
}


