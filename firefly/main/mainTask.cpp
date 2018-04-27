
#include "mainTask.h"

#include "workFacilitator.h"
#include "power/powerAdjuster.h"


// radioSoC
#include <drivers/mcu.h>
#include <clock/clockFacilitator.h>

// sleepSyncAgent
#include <syncAgent/syncAgent.h>
#include <modules/syncPowerManager.h>
#include <schedule/syncSchedule.h>



void MainTask::onReset(void) {
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
	MCU::clearResetReason();

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
	// Obsolete: ClockFacilitator::startLongClockWithSleepUntilRunning();
	ClockFacilitator::startLongClockNoWaitUntilRunning();
#endif

	// We can use Timer even though it is not accurate

	// So we can measure power.
	SyncPowerManager::init();

	// ensure Clock running (Timer is init) and SyncPowerManager can measure power
}




void MainTask::scheduleCheckPowerReserve() {
	// Arrange callback
	SyncAgent::connectOnMuchPowerReserve(MainTask::onMuchPowerReserve);

	SyncSchedule::checkPowerReserve();
	/*
	 * assert SSTask::checkPowerReserve is scheduled.
	 * When power reserve is much, will callback onMuchPowerReserve
	 */
}





void MainTask::onMuchPowerReserve() {
	//Phase::set(PhaseEnum::firstSyncPower);

	// app init
	WorkFacilitator::init();

	SyncAgent::connectApp(
			WorkFacilitator::outMailbox(),
			WorkFacilitator::onWorkSyncMsg,
			WorkFacilitator::onPreSyncSlot);

#ifndef POWER_IS_SOLAR
#pragma message("POWER unconstrained, fishing many slots")
	PowerAdjuster::setUnconstrainedEnergy();
#else
#pragma message("POWER_IS_SOLAR, fishing one slot")
#endif
	// app is init
	// sleepSyncAgent will soon init
}
