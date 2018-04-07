
cmake_minimum_required(VERSION 3.5.1)

project(sniffer C CXX ASM)



# brief
# app that runs on DK and listens continually, logging all messages
# Does not use SD (logs SleepSync, not BT.)
# Runs on nrf52832
# Uses radioSoC to listen
# Uses sleepSyncAgent for definitions of message types



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
nRF5SetIncludePaths()

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

set_target_properties( sniffer
    PROPERTIES
       CHIP "nrf52832_xxaa"
       SOFTDEVICE "none"
       FLOAT_ABI "soft"
    )
nRF5CheckTargetProperties(sniffer)
nRF5ConfigTargetByProperties(sniffer)
nRF5SetTargetStartupSources(sniffer)
nRF5SetTargetsLinkerScript(sniffer "${CMAKE_SOURCE_DIR}/gcc_nrf52NoSD.ld")






# build libs outside project
# This builds libraries if needed, and brings their targets into current scope (import not needed)
add_subdirectory( /home/bootch/git/embeddedMath embeddedMath52)
add_subdirectory( /home/bootch/git/nRF5x nRF5x52)
add_subdirectory( /home/bootch/git/radioSoC radioSoC52NoSD)
add_subdirectory( /home/bootch/git/sleepSyncAgent sleepSyncAgent52xNoSD)





# OBSOLETE since now using subdirectories
# import libs from outside project


#add_library(embeddedMath STATIC IMPORTED)
#set_target_properties( embeddedMath 
#    PROPERTIES 
#        IMPORTED_LOCATION /home/bootch/git/embeddedMath/Debug52/libembeddedMath52.a
#        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/embeddedMath/src"
#    )

#add_library(nRF5x STATIC IMPORTED)
#set_target_properties( nRF5x PROPERTIES IMPORTED_LOCATION /home/bootch/git/nRF5x/Debug52/libnRF5x52.a )

#add_library(radioSoC52 STATIC IMPORTED)
#set_target_properties( radioSoC52 
#    PROPERTIES 
#        IMPORTED_LOCATION /home/bootch/git/radioSoC/Debug52/libradioSoC52.a
#        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/radioSoC/src/"
#   )

#add_library(sleepSyncAgentM4 STATIC IMPORTED)
#set_target_properties( sleepSyncAgentM4 
#    PROPERTIES 
#         IMPORTED_LOCATION /home/bootch/git/sleepSyncAgent/ArchiveArmM4/libsleepSyncAgentM4.a
#         INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/sleepSyncAgent/src"
#    )





target_link_libraries(sniffer 
PRIVATE
    sleepSyncAgent52xNoSD
    radioSoC52NoSD
    nRF5x52
    embeddedMath52
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
       "${SDK_SOURCES}"
   )
 
 
nRF5GenerateOtherArtifacts(sniffer)
nRF5AddCustomTargets(sniffer)

message("SDK_SOURCES for sniffer: ${SDK_SOURCES}")
