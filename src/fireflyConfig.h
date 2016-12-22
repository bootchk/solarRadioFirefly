

/*
 * Is the power supply solar?
 * Else it is a battery, essentially unlimited and regulated.
 *
 * If defined,
 * firefly will manage work:
 * - use local work to limit Vss/Vcc to a maximum
 * - not do work if Vss/Vcc is below a threshold
 *
 * If using a battery, comment this out.
 *
 * If using solar power and the solar cells can generate voltages about nRF52 Vmax of 3.6V,
 * then you should uncomment this, else nRF52 can be stressed by Vcc above Vmax
 * (lowers lifetime but doesn't burn up the chip immediately.)
 */
// #define POWER_IS_SOLAR 1
