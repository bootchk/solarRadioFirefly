Files in this directory

	define ISR vector tables and other startup functions
	are specific to a target 52
	are source files to be compiled
	
	one is linked from the SDK
	other is copied from SDK hacked, and suffix changed to .cpp so I don't need to define flags for C compiler only C++
	
	
	In older SDKv12 the suffixes are hacked (.c=>.cpp, .s=>.S)
	are conditionally compiled by a C/C++ General>Paths and Symbols>Source Location (path) in a build configuration
	
	When LOGGING defined need:
	app_util_platform.c => .cpp
	
	and add include paths to ".h and app_error.h and nrf_error.h
	components/libraries/util
	coponents/nrf_drivers/nrf_soc_nosd