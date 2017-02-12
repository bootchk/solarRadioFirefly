
#include "nRF5x.h"

#include "worker.h"


/*
 * Coordinates synchronous work.
 *
 */

class GroupWork {
public:
	static void init(Mailbox* aMailbox, Worker* aLocalWorker);
	/*
	 * Definitely work local, and tell group to work if power permits.
	 */
	static void initiateGroupWork();

	/*
	 * At random, work local and tell group to work if power permits.
	 */
	static void randomlyInitiateGroupWork();

	/*
	 * Work locally, don't tell others.
	 */
	static void workInIsolation();
};
