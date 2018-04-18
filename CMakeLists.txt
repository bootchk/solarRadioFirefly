
cmake_minimum_required(VERSION 3.5.1)


# cross compiling
# set compiler earlier to avoid issues with cmake infinite loops rebuilding cache


# Use cmake modules nRF5 specific
set(CMAKE_MODULE_PATH "/home/bootch/git/nRF5Cmake/")
message("${CMAKE_MODULE_PATH}")


# environment (location of NRF_SDK and other tools)
# i.e. file CMakeEnv.cmake on the MODULE_PATH
include("CMakeEnv")

# scripts specific to nRF5x building
# Defines cmake macros prefixed with "nRF5"
# Sets many cmake vars for tool flags
include("nRF5")

nRF5CheckSetupPreconditions()
nRF5SetBuildOptions()

# assert cmake compiler flags set



# build libs outside project
# This builds libraries if needed, and brings their targets into current scope (import not needed)
add_subdirectory( /home/bootch/git/embeddedMath embeddedMath52)
add_subdirectory( /home/bootch/git/nRF5x nRF5x52)
#add_subdirectory( /home/bootch/git/radioSoC radioSoC52NoSD)
add_subdirectory( /home/bootch/git/sleepSyncAgent sleepSyncAgent52xNoSD)

add_subdirectory( /home/bootch/git/libBLEProvisionee BLEProvisionee)

#set(installBase Foo)
#include("${CMAKE_INSTALL_PREFIX}/lib/cmake/embeddedMathTargets.cmake")


# apps: delegate to subdirs

# app: niffer for sleepSync protocol
add_subdirectory(sniffer)

# app: firefly 
add_subdirectory(firefly)
