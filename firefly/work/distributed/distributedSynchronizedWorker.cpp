

#include "distributedSynchronizedWorker.h"

#include "../worker.h"

// sleepSyncAgent
#include <modules/syncPowerManager.h>	// SyncPowerManager


void DistributedSynchronizedWorker::onWorkAlarm() {
	if (SyncPowerManager::isPowerForWork())
		Worker::workManagedAmount();
}
