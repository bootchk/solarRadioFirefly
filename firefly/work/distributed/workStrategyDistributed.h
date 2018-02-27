
#pragma once


class WorkStrategyDistributed {
public:
	/*
	 * Work periodically on a schedule that is synchronized with the clique,
	 * but doesn't require a "work now" from the master,
	 * only message from master to synchronize the work schedule.
	 */
	static void manageWorkSynchronizedDistributed();

	static void provisionWork();
};
