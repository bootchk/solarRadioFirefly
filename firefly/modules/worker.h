#pragma once

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
	/*
	 * For an LED:
	 *
	 * One unit is barely perceivable flash (0.6mSec, 20 ticks).  You must be staring directly at LED.
	 * It depends very much on many factors (angle, efficiency of LED, current through the LED, and ambient light.)
	 *
	 * The rest of these comments are for a high-efficiency LED with say 15 degree angle and 1 mA current.
	 *
	 * Amount 20 gives 12mSec flash (400 ticks), more perceivable.  But still staring directly at LED.
	 *
	 * Amount 100 gives 60mSec flash (2000 ticks).
	 *
	 * Amount 200 gives 0.12Sec, hard to miss, can see out of corner of eye even in bright ambient.
	 *
	 * A sync slot is say 50 ticks @ 10mA = 500 tick*mA
	 * A LED flash of 2000 ticks @ 1mA (3V through 3K resistor) is 2000 tick*mA
	 *
	 *
	 * TODO for a piezo buzzer?
	 */

	static const unsigned int AmountBarelyPerceivable = 1;
	static const unsigned int AmountEasilyPerceivable = 50;
	static const unsigned int AmountMoreThanPerceivable = 200;


	static void init(unsigned int initialAmount);

	static void workManagedAmount();
	static void workAmount(unsigned int aAmount);


	/*
	 * Amount the least that is visible with careful observation, i.e. 1 flash of 0.6 mSec.
	 */
	static void setLeastAmount();

	static void increaseAmount();
	static void maintainAmount();
	static void decreaseAmount();

	static void setManagedAmount(unsigned int amount);
};
