

#include <cstdlib>	// rand

#include "groupWork.h"

#include "nRF5x.h"	// Mailbox


// dummy workpayload represents version of code
#define WORK_VERSION 34


namespace {

Mailbox* myOutMailbox;
Mailbox* myInMailbox;

void tellOthersInGroupToWork() {
	if (myOutMailbox->isMail() ){
		// My last mail didn't go out yet
		log("Mail still in mailbox\n");
	}
	else {
		myOutMailbox->put(WORK_VERSION);
		log("App put work\n");
	}
}

void queueLocalWork() {
	if (myInMailbox->isMail() ){
			// Work already pending from others
			log("Mail still in mailbox\n");
		}
		else {
			myInMailbox->put(WORK_VERSION);
			log("App put work\n");
		}
}

} // namespace



void GroupWork::init(Mailbox* aOutMailbox, Mailbox* aInMailbox){
	myOutMailbox = aOutMailbox;
	myInMailbox = aInMailbox;

}

void GroupWork::initiateGroupWork() {
	// assert I have enough power to work

	tellOthersInGroupToWork();

	// Power may have been exhausted by transmission

	/*
	 * But we don't receive our own transmission.
	 * So tell self to work.
	 * Work may occur later.
	 * Might not be enough power for local work.
	 */
	queueLocalWork();
}

void GroupWork::randomlyInitiateGroupWork() {
	if (rand() % 10 == 1) {
		initiateGroupWork();
	}
}



