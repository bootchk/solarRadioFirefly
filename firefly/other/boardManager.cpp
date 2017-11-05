
#include <cassert>
#include <radioSoC.h>	// LEDService

#include "../other/boardManager.h"

// Config depends on BOARD_xxx defined in build configuration


void BoardManager::initLEDs() {
#if BOARD_NRF52DK
	// nRF52DK board (from pca10040.h)
	LEDService::init(4, true, 17, 18, 19, 20);
#elif BOARD_REDBEAR_NANO || BOARD_WAVESHARE
	LEDService::init(1, McuSinks, 19, 0, 0, 0);
#elif BOARD_UBLOX_NINA_SOURCE_LED_REV1
	// uBlox, one LED, source, my footprint pin 3, UBlox pin 16, NRF52 P0.28
	LEDService::init(1, false, 28, 0, 0, 0);
#elif BOARD_UBLOX_NINA_SINK_LED
	// uBlox, one LED, source, my footprint pin 4, UBlox pin 17, NRF52 P0.29
	// July 2017 my test board, this doesn't work.  Must be a board problem.
	LEDService::init(1, McuSinks, 29, 0, 0, 0);
#elif BOARD_UBLOX_NINA_SINK_LED_REV2
	// uBlox, one LED, sunk, my footprint pin 13, UBlox pin 29, NRF52 P0.10
	LEDService::init(1, true, 10, 0, 0, 0);
#elif BOARD_WAVESHARE2_REV1
	LEDService::init(1, true, 30, 0, 0, 0);
#elif BOARD_WAVESHARE2_REV2
	LEDService::init(1, true, 8, 0, 0, 0);

#else
#error "No board defined"
#endif
assert(LEDService::wasInit());
}
