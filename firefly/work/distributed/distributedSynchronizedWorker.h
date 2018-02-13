
#pragma once

/*
 * Knows that every unit has own clock,
 * so work is just local, without sending to others.
 */
class DistributedSynchronizedWorker {
public:

	static void onWorkAlarm();

};
