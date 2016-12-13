
/*
 * Example main.
 * Tests using a c++ static library implementing LongClockTimer
 * With debug logging using Segger RTT.
 */

// API of static library
#include "nRFCounter.h"

#include "SEGGER_RTT.h"

#include "nrf.h"	// SEV


void logLongLong(uint64_t value ){
	// Print on one line
	(void) SEGGER_RTT_printf(0, "x%04x", *(((uint32_t*) &value) + 1)  );	// MS word
	(void) SEGGER_RTT_printf(0, "%04x\n", value);	// LS word and newline

}


LongClockTimer longClockTimer;

// Nothing, just wakes us.
void timeoutCallback0() {SEGGER_RTT_printf(0, "Timeout 0\n");}
void timeoutCallback1() {SEGGER_RTT_printf(0, "Timeout 1\n");}
void timeoutCallback2() {SEGGER_RTT_printf(0, "Timeout 2\n");}



void sleep() {
	__SEV();
	__WFE();	// Since internal event flag is set, this clears it without sleeping
	__WFE();	// This actually sleeps until event
}


void timerPattern0() {
	// Keep all timers running
	// Expires 1,1,0,1,1,0,2,.... more or less
	if (! longClockTimer.isTimerStarted(0)) {
		longClockTimer.startTimer(0, 20000, timeoutCallback0 );
	}

	if (! longClockTimer.isTimerStarted(1)) {
		longClockTimer.startTimer(1, 10000, timeoutCallback1 );
	}

	if (! longClockTimer.isTimerStarted(2)) {
		longClockTimer.startTimer(2, 40000, timeoutCallback2 );
	}
}


void timerPattern1() {
	// Should expire 0,1,2,0,1,2,...
	if (! longClockTimer.isTimerStarted(0)) {
		longClockTimer.startTimer(0, 20000, timeoutCallback0 );
	}
	else longClockTimer.cancelTimer(0);

	if (! longClockTimer.isTimerStarted(1)) {
		longClockTimer.startTimer(1, 10000, timeoutCallback1 );
	}
	else longClockTimer.cancelTimer(1);

	if (! longClockTimer.isTimerStarted(2)) {
		longClockTimer.startTimer(2, 40000, timeoutCallback2 );
	}
}

int main() {
	// assert embedded system startup is done and calls main.

	longClockTimer.init();

	while (true) {
		// delay
		for(int i=0; i++; i<10000) {
			int j = 0;
			j++;
		}

		logLongLong( longClockTimer.getNowTime() );

		timerPattern1();

		sleep();
		// woken by timeout or other event (rollover of LongClock)
	}

	return 0;
}
