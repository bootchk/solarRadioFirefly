


#include "nRF5x.h"


#include "worker.h"


/*
 * Coordinates synchronous work.
 *
 */

class GroupWork {
public:
	static void init(Mailbox* aMailbox, Worker* aLocalWorker);
	static void initiateGroupWork();
	static void randomlyInitiateGroupWork();
	static void workInIsolation();
};
