
#pragma once

/*
 * Knows that every unit has own clock,
 * so work is just local, without sending work to others.
 */
/*
 * !!! Each unit's worker can decide whether is power to work.
 * Does not depend on master having power to work:
 * master may be able to keep workClocks in sync without themselves having power to work.
 */
class DistributedSynchronizedWorker {
public:

	static void onWorkAlarm();
};
