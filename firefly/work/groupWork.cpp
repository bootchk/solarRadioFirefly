
// radioSoC library
#include <services/logger.h>

#include "groupWork.h"




namespace {

Mailbox* myOutMailbox;
Mailbox* myInMailbox;

void tellOthersInGroupToWork(MailContents work) {
	// RTTLogger::log("Que work out\n");
	if ( ! myOutMailbox->tryPut(work) ){
		RTTLogger::log("Fail mail group work\n");
	}
}

} // namespace


void GroupWork::queueLocalWork(MailContents work) {
	//log("App queue received or initiated work\n");
	if ( ! myInMailbox->tryPut(work) ) {
		RTTLogger::log("Fail mail self work\n");
	}
}


void GroupWork::init(Mailbox* aOutMailbox, Mailbox* aInMailbox){
	myOutMailbox = aOutMailbox;
	myInMailbox = aInMailbox;

}

void GroupWork::initiateGroupWork(MailContents work) {
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




