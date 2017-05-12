
#include "nRF5x.h"

#include "worker.h"


/*
 * Coordinates synchronous work.
 *
 * Does not know the worker, only a queue to local worker
 */

class GroupWork {
public:
	static void init(Mailbox* aOutMailbox, Mailbox* aInMailbox);
	/*
	 * Definitely work local, and tell group to work if power permits.
	 */
	static void initiateGroupWork();

	/*
	 * At random, work local and tell group to work if power permits.
	 */
	static void randomlyInitiateGroupWork();

};
