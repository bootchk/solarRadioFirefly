#pragma once

/*
 * Abstracts work, i.e. defines:
 *  - what
 *  - when to work
 *
 * Typically:
 * - flashing an LED
 * - some other transducer, sensing or driving
 *
 * Collaborates with WorkAmount to know "how much"
 */



class Worker {
public:

	// work what WorkAmount tells us
	static void workManagedAmount();

	// work given amount
	static void workAmount(unsigned int aAmount);

	// max amount we will work
	static unsigned int maxWorkAmount();
};
