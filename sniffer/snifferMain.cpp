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

/*
 * Build notes:
 * Requires libnRF52.1 built with logging enabled (build config named loggingnRF52)
 * but the library in both configurations is named the same libnRF52.1  FIXME change names
 */
// c++ includes
#include <cassert>

// Project has include path to this
#include <radioSoC.h>

// Build config include path NRF_SDK/external/segger_rtt
#include <SEGGER_RTT.h>

#include <services/logger.h>

// I couldn't get logPrintf to work

RTTLogger logger;

Radio radio;
Sleeper sleeper;
LEDService ledLogger;
BrownoutRecorder brownoutManager;


void onRcvMsgCallback() {
	;	// Do nothing, we get message in raw buffer elsewhere below
}


void logIDAndWork(BufferPointer buffer) {
	SEGGER_RTT_printf(0, "ID %02x%02x Work %02x",  buffer[2],buffer[1], buffer[10]);
}

void logRSSI() {
	int rssi = radio.receivedSignalStrength();
	SEGGER_RTT_printf(0, " RSSI %d", rssi);
}

/*
 * Understands format of messages intended for SleepSync logical layer of protocol.
 *
 * To keep overhead on RTT low, we only print LSB substring of MasterID
 *
 * buffer[2], buffer[1] is LSB of MasterID
 * buffer[10] (last byte of buffer) is the work byte
 */
void logMessage() {
	BufferPointer buffer;

	buffer = radio.getBufferAddress();

	// switch on message type
	// should include libSleepSyncAgent and cast (MessageType)
	switch( buffer[0]){

	case 17:
		// Print two LSB's of MasterID
		SEGGER_RTT_printf(0, "\n Sync ");
		logIDAndWork(buffer);
		break;
	case 136:
		SEGGER_RTT_printf(0, "\nWSync ");
		logIDAndWork(buffer);
		break;
	case 34:
		SEGGER_RTT_printf(0, "\nMSync ");
		logIDAndWork(buffer);
		break;
	case 231:
		SEGGER_RTT_printf(0, "\nInfo ");
		logIDAndWork(buffer);
		break;
	default:
		// Other unidentified i.e. garbled message types
		SEGGER_RTT_printf(0, "\n T %02x ID %02x%02x", buffer[0], buffer[2], buffer[1]);
	}



	if (radio.isPacketCRCValid()) {
		logRSSI();
	}
	else {
		// Print indicator to right of message printout
		logger.log("<-crc");
		ledLogger.toggleLED(3);
	}
}


void snifferMain(void)
{
	logger.init();

	ClockFacilitator::startLongClockWithSleepUntilRunning();

	// Radio ensemble
	Ensemble::init(sleeper.msgReceivedCallback);

#ifdef OLD
	radio.init();

	// Radio callback to Sleeper to set reasonForWake
	radio.setMsgReceivedCallback(sleeper.msgReceivedCallback);
#endif

	// LED config for nrf52DK board
    ledLogger.init(4, true, 17, 18, 19, 20);
    ledLogger.toggleLEDsInOrder();	// off

    logger.log("Sniffer starts\r\n");

    // Radio always on
    logger.log(LongClock::nowTime());
    ClockFacilitator::startHFXOAndSleepUntilRunning();	// radio requires
    //logInt(TimeMath::clampedTimeDifferenceToNow(foo));
    logger.log(LongClock::nowTime());
    logger.log("<hfclock\n");

    while (true)
    {
    	// This is not isPowerOn() because we leave radio power on
    	assert(!radio.isInUse());	// powerOn (initial entry) and stopReceive (loop) ensures this

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

    		// Even bad CRC messages can be logged.
    		logMessage();
    		break;

    	case ReasonForWake::SleepTimerExpired:
    		/*
    		 * Indicate alive but timer expired with no message.
    		 * This is normal, it will print e.g.  ....WorkSync meaning 4 timer expirations and then a WorkSync message
    		 */
    		logger.log(".");
    		ledLogger.toggleLED(1);
    		// Put radio in state that next iteration expects.
    		radio.stopReceive();
    		break;


    	case ReasonForWake::Cleared:
    		logger.log("\nUnexpected: sleep ended but no ISR called.\n");
    		// Put radio in state that next iteration expects.
    		radio.stopReceive();
    		break;

    	case ReasonForWake::CounterOverflowOrOtherTimerExpired:
    		// Every nine minutes
    		logger.log("Clock overflow.");
    		radio.stopReceive();
    		break;

    	case ReasonForWake::BrownoutWarning:
    	case ReasonForWake::HFClockStarted:
    	case ReasonForWake::LFClockStarted:
    	case ReasonForWake::Unknown:	// log("Unexpected: ISR called but no events.\n");
    		logger.log("\nUnexpected reason for wake.\n");
    		logger.log((uint32_t) sleeper.getReasonForWake());
    		//assert(false); // Unexpected
    		// Put radio in state that next iteration expects.
    		radio.stopReceive();
    		break;
    	}

    	// assert radio still on but not receiving
    	// continue loop to listen again
    }
}

