
#include "workTime.h"



// lib radioSoC
//#include <clock/longClock.h>

namespace {

DeltaTime _periodTimeToWork = 40;
}

DeltaTime WorkTime::periodTimeToWork() {
	return _periodTimeToWork;
}


void WorkTime::setPeriodTimeToWork(DeltaTime offsetFromSyncPoint) {
	_periodTimeToWork = offsetFromSyncPoint;
}
