
cmake_minimum_required(VERSION 3.5.1)

project(sniffer C CXX ASM)


# Use cmake modules nRF5 specific
set(CMAKE_MODULE_PATH "/home/bootch/git/nRF5Cmake/")
message("${CMAKE_MODULE_PATH}")


# environment (location of NRF_SDK and other tools)
# i.e. file CMakeEnv.cmake on the MODULE_PATH
include("CMakeEnv")

# set var that configures CMake_nRF5x
set(NRF_TARGET "nrf52")
# SD not used
#set(NRF_SOFTDEVICE "s132")


# scripts specific to nRF5x building
# Defines cmake macros prefixed with "nRF5"
# Sets many cmake vars for tool flags
include("nRF5")


nRF5CheckSetupPreconditions()
nRF5SetBuildOptions()
nRF5SetIncludePaths()

# SDK sources for startup, 
nRF5SDKSourcesStartup()

# Since using RTT logger need nrf libraries/util
nRF5SDKSourcesUtils()

include ("nRF5SourcesOptional")
nRF5x_addRTT()

# not SD
#nRF5SetSDKSourcesBasic()
#nRF5SDKSourcesBLE()



# Primary artifact
# Executable cross compiled for embedded NRF52
add_executable(sniffer "")


# import libs from outside project

add_library(radioSoC52 STATIC IMPORTED)
set_target_properties( radioSoC52 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/radioSoC/Debug52/libradioSoC52.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/radioSoC/src/"
    )

add_library(nRF5x STATIC IMPORTED)
set_target_properties( nRF5x PROPERTIES IMPORTED_LOCATION /home/bootch/git/nRF5x/Debug52/libnRF5x52.a )

add_library(embeddedMath STATIC IMPORTED)
set_target_properties( embeddedMath 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/embeddedMath/Debug52/libembeddedMath52.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/embeddedMath/src"
    )

add_library(sleepSyncAgentM4 STATIC IMPORTED)
set_target_properties( sleepSyncAgentM4 
    PROPERTIES 
         IMPORTED_LOCATION /home/bootch/git/sleepSyncAgent/ArchiveArmM4/libsleepSyncAgentM4.a
         INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/sleepSyncAgent/src"
    )



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




target_link_libraries(sniffer 
PRIVATE
    sleepSyncAgentM4
    radioSoC52
    nRF5x
    embeddedMath
    )


# Not used 
#    NRFDrivers
#    BLEProvisionee

   
# sources of executable
#     main which calls libraries
#     SDK startup sources
#     SDK sources for RTT and related
# all other SDK sources are built into libraries

target_sources(sniffer
   PUBLIC
       "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/snifferMain.cpp"
       "${SDK_SOURCES_STARTUP}"
       "${SDK_SOURCES}"
   )
 
 
nRF5GenerateOtherArtifacts(sniffer)
nRF5AddCustomTargets(sniffer)

message("SDK_SOURCES_STARTUP: ${SDK_SOURCES_STARTUP}")
message("SDK_SOURCES: ${SDK_SOURCES}")
