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
 * - so that this clique workw at same time as other cliques
 *   so that all cliques work in sync (as well as members of clique work in sync.)
 *
 */
class WorkTime {
public:
	// Ticks from syncPoint til time to work
	static DeltaTime periodTimeToWork();

	/*
	 *  Given a TOA (provisioning time) and an offset from a mark time (from the provisioning),
	 *  set the work time.
	 *
	 *  Here assume TOA is now time, i.e. this is called at time of provisioning.
	 */

	static void setPeriodTimeToWork(DeltaTime offsetFromSyncPoint);
};
