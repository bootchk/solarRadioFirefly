/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */


#pragma once

/*
 * lkk
 * The Eclipse build configuration (in Properties>C/C++ Build>Settings>Preprocessor>Defined Symbols:)
 * defines either BOARD_PCA10040 or BOARD_CUSTOM.
 * The former causes include from SDK of a file custom to the nRF52DK dev board.
 * The latter causes include this file for RedBear Nano or other custom board.
 *
 * The path to this file must also be in the build configuration (in Properties>C/C++ Build>Settings>Preprocessor>Includes
 *
 * The numbers are GPIO numbers (ordinals of the bits in the GPIO register)  ???
 * Not pin numbers on the chip.
 */




// change def in fireflyConfig to choose a custom board
#include "fireflyConfig.h"




#ifdef BOARD_REDBEAR_NANO

#define LEDS_NUMBER    1
// pin 39, GPIO_19
#define LED_START  19
#define BSP_LED_0  19
#define LED_STOP   19
#define BSP_LED_0_MASK    (1<<BSP_LED_0)
#define LEDS_MASK      (BSP_LED_0_MASK)
// this means is a sink, pin low means LED is lit
#define LEDS_INV_MASK  LEDS_MASK
#endif


#ifdef BOARD_UBLOX_NINA_SOURCE_LED
// custom board with Ublox GPIO_16 connected to LED to ground (nrf52 sources it)
#define LEDS_NUMBER    1

#define LED_START  28
#define BSP_LED_0  28
#define LED_STOP   28
#define BSP_LED_0_MASK    (1<<BSP_LED_0)
#define LEDS_MASK      (BSP_LED_0_MASK)
// this means is a source, pin low means LED is not lit
#define LEDS_INV_MASK  ~LEDS_MASK
#define LEDS_LIST { BSP_LED_0 }
#endif


#ifdef BOARD_UBLOX_NINA_SINK_LED
// custom board with Ublox GPIO_17 connected to LED to VCC (nrf52 sinks it)
// pin ???, Nordic P0.29
#define LEDS_NUMBER    1

#define LED_START  29
#define BSP_LED_0  29
#define LED_STOP   29
#define BSP_LED_0_MASK    (1<<BSP_LED_0)
#define LEDS_MASK      (BSP_LED_0_MASK)
// this means is a sink, pin low means LED is lit
#define LEDS_INV_MASK  LEDS_MASK
#define LEDS_LIST { BSP_LED_0 }
#endif


#ifdef BOARD_LEDS_NONE
// a custom board with no LED's (or configured to not use them)
#define LEDS_NUMBER    0

// ?????
#define LED_START  28
#define BSP_LED_0  28
#define LED_STOP   28

#define LEDS_MASK      (0)
// this means is a source, pin low means LED is not lit
#define LEDS_INV_MASK  ~LEDS_MASK
#define LEDS_LIST {}
#endif


// used by bsp.c
// BSP_LED_0_MASK    defined above
// bsp.c assumes BSP_LED_1_MASK always exists
#define BSP_LED_1_MASK    (1<<BSP_LED_0)


// there are no buttons on this board
#define BUTTONS_LIST {}
#define BUTTONS_NUMBER 0
#define BUTTONS_MASK   0x00000000


// UART pins connected to J-Link
#define RX_PIN_NUMBER  11
#define TX_PIN_NUMBER  9
#define CTS_PIN_NUMBER 10
#define RTS_PIN_NUMBER 8
#define HWFC           true


