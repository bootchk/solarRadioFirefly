
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

# BOARD_NRF52DK


# scripts specific to nRF5x building
# Defines cmake macros prefixed with "nRF5"
# Sets many cmake vars for tool flags
include("nRF5")


nRF5CheckSetupPreconditions()
nRF5SetBuildOptions()
nRF5SetIncludePaths()
nRF5SDKSourcesStartup()
# nRF5SetSDKSourcesBasic()

# Also BLE
nRF5SDKSourcesBLE()



# Primary artifact
# Executable cross compiled for embedded NRF52
add_executable(fireflyInteractive "")


# import libs from outside project

add_library(radioSoC52wSD STATIC IMPORTED)
set_target_properties( radioSoC52wSD 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/radioSoC/Debug52wSD/libradioSoC52wSD.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/radioSoC/src"
    )

add_library(nRF5x STATIC IMPORTED)
set_target_properties( nRF5x 
	PROPERTIES IMPORTED_LOCATION /home/bootch/git/nRF5x/Debug52/libnRF5x52.a
	INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/nRF5x/src"
	)

add_library(embeddedMath STATIC IMPORTED)
set_target_properties( embeddedMath PROPERTIES IMPORTED_LOCATION /home/bootch/git/embeddedMath/Debug52/libembeddedMath52.a )

add_library(NRFDrivers STATIC IMPORTED)
set_target_properties( NRFDrivers PROPERTIES IMPORTED_LOCATION /home/bootch/git/libNRFDrivers/Debug/libNRFDrivers.a )

add_library(BLEProvisionee STATIC IMPORTED)
set_target_properties( BLEProvisionee 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/libBLEProvisionee/Debug/libBLEProvisionee.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/libBLEProvisionee/objects"
    )

add_library(sleepSyncAgent STATIC IMPORTED)
set_target_properties( sleepSyncAgent 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/sleepSyncAgent/DebugProvisioned/libsleepSyncAgentM4Provisioned.a
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



# ??? order is important unless we specify interlibrary dependencies
target_link_libraries(fireflyInteractive 
PRIVATE
    sleepSyncAgent
    radioSoC52wSD
    nRF5x
    embeddedMath
    BLEProvisionee
    NRFDrivers
    )

# TODO firefly non-interactive
# TODO radioSoC52SD
# TODO sleepSync

   
# sources of executable
#     main which calls libraries
#     SDK startup sources
# all other SDK sources are built into libraries

target_sources(fireflyInteractive
   PUBLIC
       "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/workFacilitator.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/powerManagedMain.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/work/workSupervisor.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/work/parameters/workFrequency.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/work/parameters/workAmount.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/work/distributed/workStrategyDistributed.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/work/distributed/workClock.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/work/distributed/distributedSynchronizedWorker.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/work/distributed/workSyncMaintainer.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/work/distributed/workSyncMaintenanceClock.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/work/worker.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/power/powerAdjuster.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/other/boardManager.cpp"
       "${SDK_SOURCES_STARTUP}"
   )
 
 
nRF5GenerateOtherArtifacts(fireflyInteractive)

# not generate FLASH_SOFTDEVICE target, already created by sniffer
# nRF5AddCustomTargets(fireflyInteractive)


