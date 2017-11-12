
#pragma once

/*
 * Knows how to shed power
 * to prevent Vcc > Vmax == 3.6V
 *
 * Pure class
 */

class PowerShedder {
public:
	static void shedPowerUntilVccLessThanVmax();
};
