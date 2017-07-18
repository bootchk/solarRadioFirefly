

#include <cstdlib>	// rand

#include "groupWork.h"

#include "nRF5x.h"	// Mailbox





namespace {

Mailbox* myOutMailbox;
Mailbox* myInMailbox;

void tellOthersInGroupToWork(WorkPayload work) {
	if (myOutMailbox->isMail() ){
		// My last mail didn't go out yet
		log("Mail still in Out mailbox\n");
	}
	else {
		myOutMailbox->put(work);
		//log("App put work\n");
	}
}

} // namespace


void GroupWork::queueLocalWork(WorkPayload work) {
	if (myInMailbox->isMail() ){
		// Work already pending from others
		log("Mail still in In mailbox\n");
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

void GroupWork::randomlyInitiateGroupWork(WorkPayload work) {
	if (rand() % 10 == 1) {
		initiateGroupWork(work);
	}
}



