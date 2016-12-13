
#include <inttypes.h>


/*
 * Type LongClock yields.  Only 56 bits are valid.
 */
typedef uint64_t LongTime;




/*
 * Thin wrapper around RTC device of Nordic nRF52/51.
 * This defines an API that might be portable.
 *
 * Owns private data defined in anon namespace.
 *
 * Provides:
 * - LongClock 56-bit counter. At resolution of 30uSec, overflows in 67,000 years.
 * - Three Timers, 24-bit, one-shot. At resolution of 30uSec, max timeout is 5 seconds.
 *
 * !!! Timers are shorter than the LongClock.
 * 24-bit RTC device counter underlies both LongClock and Timers,
 * but LongClock extends the clock to 56-bits.
 *
 * Legal call sequences:
 *
 * Normal:  init, getNowTime, startTimer, cancelTimer
 * You can cancel a Timer that has not been started: init, cancelTimer
 */
class LongClockTimer {
	static const int OSClockCountBits = 24;
	static const unsigned int MaxTimeout = 0xFFFFFF;

public:
	/*
	 * How many Timers this device supports (with compare registers).
	 * Depends on device, here 3 is compatible with nRF51 and nRF52, certain revisions?
	 */
	static const unsigned int CountTimerInstances = 3;

	static void init();

	static LongTime getNowTime();

	static void startTimer(
			unsigned int index,	// [0:2]
			unsigned int timeout, // [0:xffffff]
			void (*onTimeoutCallback)());

	static bool isTimerStarted(unsigned int index);

	static void cancelTimer(unsigned int index);
};
