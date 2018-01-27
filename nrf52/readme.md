Files in this directory
-

	define ISR vector tables and other startup functions
	are specific to a target 52
	are source files to be compiled
	
Origins
-

gcc_startup_nrf52.S     is linked from the SDK
	
other files are copied from SDK hacked, and suffix changed to .cpp so I don't need to define flags for C compiler only C++
	
In older SDKv12 the suffixes are hacked (.c=>.cpp, .s=>.S) are conditionally compiled by a C/C++ General>Paths and Symbols>Source Location (path) in a build configuration
	
Per file notes
-
app_util_platform.c => .cpp       

When LOGGING defined need this
and add include paths to ".h and app_error.h and nrf_error.h
	components/libraries/util
	coponents/nrf_drivers/nrf_soc_nosd
	
Also needed fpr app_util_critical<foo> when SOFTDEVICE_PRESENT
	
nrf_assert.h     ??? why copied

sdk_config.h is needed when Softdevice and modules are used (e.g. nrf_drv_clock)

OLD
-

TODO eliminate other files above, except for two startup files

nrf_drv_clock is used to coordinate clock access with Softdevice.  Now it comes from libNRFDrivers

components/libraries/util/app_error.c
" app_error_weak.c    
     When multiprotocol and BLE softdevice are used, need these
     (Also need app_error_weak unless we define our own app_error_fault_handler)
