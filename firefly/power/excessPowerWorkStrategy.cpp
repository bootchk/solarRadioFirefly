

#include "excessPowerWorkStrategy.h"


#include "powerAdjuster.h"
#include "../work/groupWork/workStrategyGroup.h"
#include "../work/distributed/workStrategyDistributed.h"

// sleepSyncAgent
#include <modules/syncPowerManager.h>



namespace {

void doNestedWorkStrategy() {
	/*
	 * Choice here of nested work strategy.
	 * Similar to choice when outer strategy does not manage excess voltage.
	 */

	// 1
	//WorkStrategyGroup::manageWorkOnlyRegularlyIfPowerAndMaster();
	// 2
	WorkStrategyDistributed::manageWorkSynchronizedDistributed();
}

}



void ExcessPowerWorkStrategy::manageExcessPowerWithWork() {

	/* It is possible for power levels to drop so precipitously that
	 * on consecutive calls, the VoltageRange's are not adjacent,
	 * i.e. pass from Excess to Medium or worse.
	 */
	switch (SyncPowerManager::getVoltageRange()) {

	case VoltageRange::AboveExcess: // e.g. > 3.6V
	case VoltageRange::NearExcess: // e.g. > 3.4V
		PowerAdjuster::onExcessVoltage();
		break;

	/*
	 * Rest of cases:
	 * Adjust work amount.
	 * Self not need to work to manage excess power.
	 * Delegate to other work strategy to decide whether to work.
	 *
	 *
	 * Delegated work strategy may cause local work.
	 * Thus we might do two local works in consecutive calls.
	 * I.E. For Excess, then for High
	 */

	/* Targeted range, i.e. moderate. */
	case VoltageRange::HighToNearExcess:  // e.g. 2.7--3.4V
	case VoltageRange::MediumToHigh:   // e.g. 2.5--2.7V
		PowerAdjuster::maintainUsage();
		doNestedWorkStrategy();
		break;


	case VoltageRange::LowToMedium:  // e.g. 2.3-2.5V
		PowerAdjuster::decreaseUsage();
		doNestedWorkStrategy();
		break;


	case VoltageRange::UltraLowToLow:	// e.g. 2.1-2.3V
		/*
		 * Not enough power to work, and power getting low.
		 */
		// TODO for distributed, still need to maintain workclock ?
		PowerAdjuster::decreaseUsage();
		break;

	case VoltageRange::BelowUltraLow:	//  e.g. <2.1V
		/*
		 * Not enough power to work.
		 * Power nearing brownout.
		 * Next work done (if ever) is least amount.
		 *
		 * It is possible for power levels to drop so precipitously that
		 * we didnt' pass through case above for Low.
		 */
		/* See also brownout detector should find this case asynchronously. */
		PowerAdjuster::setLeastUsage();
		break;
	}
}
