#pragma once

#include "nRF5x.h"	// LongClockTimer

/*
 * Abstracts work, i.e. the implementation defines what "work" is.
 *
 * Typically:
 * - flashing an LED
 * - some other transducer, sensing or driving
 *
 * Amount of work is settable.
 * Units are defined in the implementation (typically units of 0.6mSec LED flash.)
 * So that work can be used to shed power.
 *
 * Pure class: no instances.
 */


class Worker {
public:
	static void init();

	static void workManagedAmount();
	static void workAmount(unsigned int aAmount);


	/*
	 * Amount the least that is visible with careful observation, i.e. 1 flash of 0.6 mSec.
	 */
	static void setLeastAmount();

	static void increaseAmount();
	static void maintainAmount();
	static void decreaseAmount();

	/*
	 * Amount enough that tester can easily perceive, when power is line power and regulated.
	 * For an LED or other transceiver.
	 */
	static void setAmountPerceivable();
};
