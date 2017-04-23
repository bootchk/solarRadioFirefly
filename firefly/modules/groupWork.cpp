

#include <cstdlib>	// rand

#include "groupWork.h"
#include "worker.h"


#include "nRF5x.h"


// dummy workpayload represents version of code
#define WORK_VERSION 34


namespace {

Mailbox* myOutMailbox;
Worker* localWorker;

void sendWork() {
	if (myOutMailbox->isMail() ){
		// My last mail didn't go out yet
		log("Mail still in mailbox\n");
	}
	else {
		myOutMailbox->put(WORK_VERSION);
		log("App put work\n");
	}
}

} // namespace



void GroupWork::init(Mailbox* aMailbox, Worker* aLocalWorker){
	myOutMailbox = aMailbox;
	localWorker = aLocalWorker;
}

void GroupWork::initiateGroupWork() {
	localWorker->workManagedAmount();
	/*
	 * Local work might exhaust power (not usual.)
	 * Assert queue will be non-empty (queuing takes no power)
	 * If on the previous cycle, we also queued but there wasn't enough power to xmit, then queue was not empty.
	 * Sending might not happen (not enough power for radio).
	 */
	sendWork();		// tell group to work
}

void GroupWork::randomlyInitiateGroupWork() {
	if (rand() % 10 == 1) {
		initiateGroupWork();
	}
}

void GroupWork::workInIsolation() {
	// work on this unit, but don't tell group
	localWorker->workManagedAmount();
}



