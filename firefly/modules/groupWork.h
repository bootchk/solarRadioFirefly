
#include <radioSoC.h>	// Mailbox


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
	static void initiateGroupWork(WorkPayload work);


	/*
	 * Queue work to be done locally.
	 * This is the only place that puts to the In queue.
	 * Called:
	 * - when work is heard
	 * - when work is initiated locally
	 */
	static void queueLocalWork(WorkPayload work);
};
