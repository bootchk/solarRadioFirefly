
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





# Rest delegated to subdirs

# Sniffer for sleepSync protocol
add_subdirectory(sniffer)

# firefly app
add_subdirectory(firefly)
