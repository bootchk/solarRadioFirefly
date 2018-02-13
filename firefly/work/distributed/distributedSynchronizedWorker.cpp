

#include "distributedSynchronizedWorker.h"

#include "../worker.h"




void DistributedSynchronizedWorker::onWorkAlarm() {
	// work regardless of power
	Worker::workManagedAmount();
}
