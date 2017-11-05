
// radioSoC library
#include <services/logger.h>

#include "groupWork.h"




namespace {

Mailbox* myOutMailbox;
Mailbox* myInMailbox;

void tellOthersInGroupToWork(WorkPayload work) {
	if (myOutMailbox->isMail() ){
		// My last mail didn't go out yet
		RTTLogger::log("Mail still in OutBox\n");
	}
	else {
		myOutMailbox->put(work);
		RTTLogger::log("Que work out\n");
	}
}

} // namespace


void GroupWork::queueLocalWork(WorkPayload work) {
	if (myInMailbox->isMail() ){
		// Work already pending from others
		RTTLogger::log("Mail still in InBox\n");
	}
	else {
		myInMailbox->put(work);
		//log("App queue received or initiated work\n");
	}
}


void GroupWork::init(Mailbox* aOutMailbox, Mailbox* aInMailbox){
	myOutMailbox = aOutMailbox;
	myInMailbox = aInMailbox;

}

void GroupWork::initiateGroupWork(WorkPayload work) {
	// assert I have enough power to work

	tellOthersInGroupToWork(work);

	// Power may have been exhausted by transmission

	/*
	 * But we don't receive our own transmission.
	 * So tell self to work.
	 * Work may occur later.
	 * Might not be enough power for local work.
	 */
	queueLocalWork(work);
}




