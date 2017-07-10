/*
 * A main() that sniffs other unit broadcasts.
 *
 * Sniff: logs valid message types.
 *
 * Assumes steady power source, but does sleep.
 * Sleeps just to wake and toggle an LED to show alive.
 *
 * LED1 alive but no message
 * LED3 message CRC invalid
 *
 * Currently specialized:
 * - logMessage understands SleepSync encoded messages
 * - uses RTT logging
 *
 * To use:
 * - start debugger in Eclipse
 * - start JLinkRTTClient which attaches to debugger and displays log of sniffed messages
 *
 * In the log:
 * - "." means a sleeping period where no message was received
 * - "Bad CRC" means message was received but CRC invalid.
 */

// c++ includes
#include <cassert>

// Project has include path to nRF5x library
#include <nRF5x.h>
// Build config include path NRF_SDK/external/segger_rtt
#include <SEGGER_RTT.h>

// I couldn't get logPrintf to work


Radio radio;
Sleeper sleeper;
LEDService ledLogger;
BrownoutRecorder brownoutManager;


void onRcvMsgCallback() {
	;	// Do nothing, we get message in raw buffer elsewhere below
}

/*
 * Understands format of messages intended for SleepSync logical layer of protocol.
 */
void logMessage() {
	BufferPointer buffer;

	buffer = radio.getBufferAddress();

	switch(buffer[0]){

	case 17:
		// Print two LSB's of MasterID
		SEGGER_RTT_printf(0, "Sync %02x%02x\r\n", buffer[2], buffer[1]);

		//logPrintf("Sync %02x%02x\r\n", buffer[1], buffer[2]);
		//log("Sync"); logByte(buffer[1]); log("\n");
		break;
	case 136:
		SEGGER_RTT_printf(0, "WorkSync %02x%02x %02X\n",  buffer[2],buffer[1], buffer[10]);
		break;
	case 34:
		SEGGER_RTT_printf(0, "MergeSync %02x%02x\n",  buffer[2],buffer[1]);
		break;
	default:
		// Other message types
		SEGGER_RTT_printf(0, "Other %02x %02x%02x\r\n", buffer[0], buffer[2], buffer[1]);
	}
}


void snifferMain(void)
{
	initLogging();

	ClockFacilitator::startLongClockWithSleepUntilRunning();

	HfCrystalClock::init();

	// Sleeper requires initialized TimerService
	// use maxSaneTimeout default

	radio.init();

	// Radio callback to Sleeper to set reasonForWake
	radio.setMsgReceivedCallback(sleeper.msgReceivedCallback);

	// LED config for nrf52DK board
    ledLogger.init(4, McuSinks, 17, 18, 19, 20);
    ledLogger.toggleLEDsInOrder();	// off

    log("Sniffer starts\r\n");

    // Radio always on
    logLongLong(LongClock::nowTime());
    HfCrystalClock::startAndSleepUntilRunning();	// radio requires
    //logInt(TimeMath::clampedTimeDifferenceToNow(foo));
    logLongLong(LongClock::nowTime());
    log("<hfclock\n");

    while (true)
    {
    	// This is not isPowerOn() because we leave radio power on
    	assert(!radio.isInUse());	// powerOn (initial entry) and stopReceiver (loop) ensures this

    	sleeper.clearReasonForWake();
    	radio.receiveStatic();
    	// Receiving with interrupt enabled.
    	assert(radio.isEnabledInterruptForMsgReceived());

    	// Sleep until msg received or timeout
    	// units of .03 uSec ticks, one second
    	sleeper.sleepUntilEventWithTimeout(30000);

    	// If using nrf52DK with many LED's show result
    	// Some interrupt ??? event woke us up and set reasonForWake
    	switch ( sleeper.getReasonForWake() ) {
    	case ReasonForWake::MsgReceived:
    		// !!! Note toggling of LED 2 usually done in radio.c on every receive
    		if (radio.isPacketCRCValid()) {
    			logMessage();
    		}
    		else {
    			log("Bad CRC: ");
    			// buffer can be dumped despite bad CRC
    			logMessage();
    			ledLogger.toggleLED(3);
    		}
    		break;

    	case ReasonForWake::SleepTimerExpired:
    		// Indicate alive but no message.
    		log(".");
    		ledLogger.toggleLED(1);
    		// Put radio in state that next iteration expects.
    		radio.stopReceive();
    		break;

    	case ReasonForWake::Unknown:
    		log("Unexpected: ISR called but no events.\n");
    		logInt((uint32_t) sleeper.getReasonForWake());
    		//assert(false); // Unexpected
    		;
    		// Put radio in state that next iteration expects.
    		radio.stopReceive();
    		break;
    	case ReasonForWake::Cleared:
    		log("Unexpected: sleep ended but no ISR called.\n");
    		// Put radio in state that next iteration expects.
    		radio.stopReceive();
    	}

    	// assert radio still on but not receiving
    	// continue loop to listen again
    }
}

