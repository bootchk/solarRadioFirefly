/*
 * Sniff other unit broadcasts.
 * Control network.
 * Specific to SleepSync protocol.
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


// To control network, need SleepSync library and include paths to embeddedMath library
#include <syncAgent/syncAgent.h>
#include <modules/syncSender.h>
#include <message/serializer.h>

// I couldn't get logPrintf to work

RTTLogger logger;

Radio radio;
Sleeper sleeper;
LEDService ledLogger;
BrownoutRecorder brownoutManager;


void onRcvMsgCallback() {
	;	// Do nothing, we get message in raw buffer elsewhere below
}

/*
 * Print 2 bytes of ID.
 * LSB: buffer[2],buffer[1]
 */
void logIDAndWork(MessageType aType, BufferPointer buffer) {
	SEGGER_RTT_printf(0, "%s ID %02x%02x Work %02x",
			SyncMessage::representation(aType),
			buffer[6],buffer[5], // MSB two bytes of ID
			buffer[10]);
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

	/*
	 * If packet is CRC error, append to line
	 * else start a new line.
	 */
	if (! radio.isPacketCRCValid()) {
		// Print indicator to left of message printout
		logger.log(" crc:");
		ledLogger.toggleLED(3);
	}
	else {
		SEGGER_RTT_printf(0, "\n");
	}

	// switch on message type
	// should include libSleepSyncAgent and cast (MessageType)
	MessageType msgType = SyncMessage::messageTypeFromRaw(buffer[0]);

	switch( msgType ){

	case MessageType::MasterSync:
	case MessageType::WorkSync:
	case MessageType::ControlNetGranularity:
	case MessageType::ControlScatterClique:
	case MessageType::ControlWorkTime:
	case MessageType::ControlWorkCycle:
	case MessageType::EnticingInferior:
	case MessageType::MasterMergedAway:
	case MessageType::SlaveMergedAway:
	case MessageType::AbandonMastership:
	case MessageType::Info:
		SEGGER_RTT_printf(0, " Sync ");
		logIDAndWork(msgType, buffer);
		break;

	case MessageType::Invalid:
		// Other unidentified i.e. garbled message types
		SEGGER_RTT_printf(0, "Type %02x ID %02x%02x", buffer[0], buffer[2], buffer[1]);
	}


	if (radio.isPacketCRCValid()) {
		logRSSI();
	}
}


void messageLoggingSleep() {
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


void sendControlMessage() {

	/*
	 * assert ensemble started but radio not in use
	 */

	/*
	 * To ensure it is heard, send it for an entire sync period, 2.1 seconds.
	 * So that it hits every unit's sync slot.
	 * E.g. 1600 slots of 1.5mSec apart
	 */
	for (int i = 1600; i>0; i--) {
		// Change xmit power
		// OBSOLETE SyncSender::sendControlGranularity(static_cast<WorkPayload>(TransmitPowerdBm::Minus8));

		// delay 1.5 mSec
		sleeper.clearReasonForWake();
		sleeper.sleepUntilEventWithTimeout(50);

	}
}



void snifferMain(void)
{
	RadioUseCase radioUseCaseSleepSync;


	logger.init();

	// Serializer required for xmit
	Serializer::init(Radio::getBufferAddress(), Radio::FixedPayloadCount);

	ClockFacilitator::startLongClockWithSleepUntilRunning();

	Ensemble::init(sleeper.msgReceivedCallback); // OLD , &radioUseCaseSleepSync);

	// In future use case can be BLE
	Ensemble::setRadioUseCase(&radioUseCaseSleepSync);

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

    // This could be: ensemble::startup();

    logger.log(LongClock::nowTime());
    logger.log("<hfclock starting...\n");

    //ClockFacilitator::startHFXOAndSleepUntilRunning();	// radio requires
    ClockFacilitator::startHFClockWithSleepConstantExpectedDelay(500);
    //logInt(TimeMath::clampedTimeDifferenceToNow(foo));

    logger.log(LongClock::nowTime());
    logger.log("<hfclock started\n");

    while (true) {

#ifdef TEST_CONTROL
    	// Listen for 40 messages at default xmit power
    	for (int i = 40; i>0; i--)
    		messageLoggingSleep();

    	// sendControlMessage();
#endif
    	messageLoggingSleep();
    	//logger.log("Listening...\n");
    }
}

