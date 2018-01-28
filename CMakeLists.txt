
cmake_minimum_required(VERSION 3.5.1)

project(fireflyInteractive C CXX ASM)


# Use cmake modules nRF5 specific
set(CMAKE_MODULE_PATH "/home/bootch/git/nRF5Cmake/")
message("${CMAKE_MODULE_PATH}")


# environment (location of NRF_SDK and other tools)
# i.e. file CMakeEnv.cmake on the MODULE_PATH
include("CMakeEnv")

# set var that configures CMake_nRF5x
set(NRF_TARGET "nrf52")
set(NRF_SOFTDEVICE "s132")


# scripts specific to nRF5x building
# Defines cmake macros prefixed with "nRF5"
# Sets many cmake vars for tool flags
include("nRF5")


nRF5CheckSetupPreconditions()
nRF5SetBuildOptions()
nRF5SetIncludePaths()
nRF5SetSDKSourcesBasic()

# Also BLE
nRF5SDKSourcesBLE()



# Primary artifact
# Executable cross compiled for embedded NRF52
add_executable(fireflyInteractive "")


# import libs from outside project

add_library(radioSoC52 STATIC IMPORTED)
set_target_properties( radioSoC52 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/radioSoC/Debug52/libradioSoC52.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/radioSoC/src/clock"
    )

add_library(nRF5x STATIC IMPORTED)
set_target_properties( nRF5x PROPERTIES IMPORTED_LOCATION /home/bootch/git/nRF5x/DebugnRF52/libnRF5x52.a )

add_library(embeddedMath STATIC IMPORTED)
set_target_properties( embeddedMath PROPERTIES IMPORTED_LOCATION /home/bootch/git/embeddedMath/Debug52/libembeddedMath52.a )

add_library(NRFDrivers STATIC IMPORTED)
set_target_properties( NRFDrivers PROPERTIES IMPORTED_LOCATION /home/bootch/git/libNRFDrivers/Debug/libNRFDrivers.a )



IF(EXISTS /home/bootch/git/radioSoC/Debug52/libradioSoC52.a)
else()
    message("FATAL_ERROR libradioSoC52.a not exist")
ENDIF()

IF(EXISTS /home/bootch/git/nRF5x/Debug52/libnRF5x52.a )
else()
    message("FATAL_ERROR libnRF5x52.a not exist")
ENDIF()

IF(EXISTS /home/bootch/git/embeddedMath/Debug52/libembeddedMath52.a)
else()
    message("FATAL_ERROR libembeddedMath52.a not exist")
ENDIF()




target_link_libraries(fireflyInteractive 
PRIVATE
    BLEProvisionee
    radioSoC52
    nRF5x
    embeddedMath
    NRFDrivers
    )

# TODO firefly
# TODO radioSoC52SD

   
# sources of executable
#     main which calls libraries
#     SDK startup sources
# all other SDK sources are built into libraries

target_sources(fireflyInteractive
   PUBLIC
       "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
       "${SDK_STARTUP_SOURCES}"
   )
 
 
nRF5GenerateOtherArtifacts(fireflyInteractive)
nRF5AddCustomTargets(fireflyInteractive)


