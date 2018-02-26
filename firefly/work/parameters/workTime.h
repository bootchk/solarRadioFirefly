#pragma once

#include <inttypes.h>

// lib SleepSync
#include "types.h"	// DeltaTime


/*
 * Knows what time to work.
 *
 * Time to work is reference to syncPoint.
 * Time to work is settable.
 *
 * The purpose of setting work time:
 * - so that this clique works at same wall time as other cliques
 *   so that all cliques work in sync (as well as members of clique work in sync.)
 */
class WorkTime {
public:
	/*
	 * Ticks from syncPoint til time to work.
	 * In a design where time to work is not necessarily aligned with sync point.
	 */
	static DeltaTime periodTimeToWork();

	/*
	 *  Set the work time, given:
	 *  - a TOA (provisioning time)
	 *  - and an offset from a mark time (from the provisioning),
	 *
	 *  Here assume TOA is now time, i.e. this is called at time of provisioning.
	 */
	static void setPeriodTimeToWork(DeltaTime offsetFromSyncPoint);


};
