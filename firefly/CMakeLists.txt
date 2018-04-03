
cmake_minimum_required(VERSION 3.5.1)

project(fireflyInteractive C CXX ASM)


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
# nRF5SetSDKSourcesBasic()

# Also BLE
# nRF5SDKSourcesBLE()



# Primary artifact
# Executable cross compiled for NRF52832
add_executable(fireflyInteractive "")

set_target_properties( fireflyInteractive
    PROPERTIES
       CHIP "nrf52832_xxaa"
       SOFTDEVICE "s132"
       FLOAT_ABI "soft"
    )
nRF5CheckTargetProperties(fireflyInteractive)
nRF5ConfigTargetByProperties(fireflyInteractive)
nRF5SetTargetStartupSources(fireflyInteractive)
nRF5SetTargetsLinkerScript(fireflyInteractive "${CMAKE_SOURCE_DIR}/gcc_nrf52.ld")



#TODO this cmake script uses SDK's std gcc_startup_nrf52.S
# whose heap size is set non-zero 
# whereas the local one I hacked

add_executable(firefly52810e "")

set_target_properties( firefly52810e
    PROPERTIES
       CHIP "nrf52810e"
       SOFTDEVICE "s112"
       FLOAT_ABI "soft"
    )
nRF5CheckTargetProperties(firefly52810e)
nRF5ConfigTargetByProperties(firefly52810e)
nRF5SetTargetStartupSources(firefly52810e)
nRF5SetTargetsLinkerScript(firefly52810e "${CMAKE_SOURCE_DIR}/linkerScripts/nrf52810SD.ld")



add_executable(firefly52810eNoSD "")

set_target_properties( firefly52810eNoSD
    PROPERTIES
       CHIP "nrf52810e"
       SOFTDEVICE "none"
       FLOAT_ABI "soft"
    )
nRF5CheckTargetProperties(firefly52810eNoSD)
nRF5ConfigTargetByProperties(firefly52810eNoSD)
nRF5SetTargetStartupSources(firefly52810eNoSD)
nRF5SetTargetsLinkerScript(firefly52810eNoSD "${CMAKE_SOURCE_DIR}/linkerScripts/nrf52810_xxaa.ld")
nRF5SetTargetRTTUtilSources(firefly52810eNoSD)

target_compile_definitions(firefly52810eNoSD PUBLIC BOARD_PCA10040)


    
add_executable(firefly52810SDFanstel "")

set_target_properties( firefly52810SDFanstel
    PROPERTIES
       CHIP "nrf52810_xxaa"
       SOFTDEVICE "s112"
       FLOAT_ABI "soft"
    )
nRF5CheckTargetProperties(firefly52810SDFanstel)
nRF5ConfigTargetByProperties(firefly52810SDFanstel)
nRF5SetTargetStartupSources(firefly52810SDFanstel)
nRF5SetTargetsLinkerScript(firefly52810SDFanstel "${CMAKE_SOURCE_DIR}/linkerScripts/nrf52810SD.ld")





# differences: no SD, linkerscript, board
add_executable(firefly52810NoSDFanstel "")

set_target_properties( firefly52810NoSDFanstel
    PROPERTIES
       CHIP "nrf52810_xxaa"
       SOFTDEVICE "none"
       FLOAT_ABI "soft"
    )
nRF5CheckTargetProperties(firefly52810NoSDFanstel)
nRF5ConfigTargetByProperties(firefly52810NoSDFanstel)
nRF5SetTargetStartupSources(firefly52810NoSDFanstel)
# RTT logging requires app_util
# TODO the rest of RTT is built into radioSoC, why isn't this?
nRF5SetTargetRTTUtilSources(firefly52810NoSDFanstel)
nRF5SetTargetsLinkerScript(firefly52810NoSDFanstel "${CMAKE_SOURCE_DIR}/linkerScripts/nrf52810_xxaa.ld")

target_compile_definitions(firefly52810NoSDFanstel PUBLIC BOARD_FANSTEL52810)



# import libs from outside project

add_library(sleepSyncAgent52xSD STATIC IMPORTED)
set_target_properties( sleepSyncAgent52xSD 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/sleepSyncAgent/cmakeBuild/src/libsleepSyncAgent52832.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/sleepSyncAgent/src"
        IMPORTED_LINK_INTERFACE_LIBRARIES embeddedMath52
    )
    
    
add_library(sleepSyncAgent52xNoSD STATIC IMPORTED)
set_target_properties( sleepSyncAgent52xNoSD 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/sleepSyncAgent/cmakeBuild/src/libsleepSyncAgent52xNoSD.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/sleepSyncAgent/src"
        IMPORTED_LINK_INTERFACE_LIBRARIES embeddedMath52
    )

# with provisioning
#IMPORTED_LOCATION /home/bootch/git/sleepSyncAgent/DebugProvisioned/libsleepSyncAgentM4Provisioned.a  

add_library(radioSoC52wSD STATIC IMPORTED)
set_target_properties( radioSoC52wSD 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/radioSoC/Debug52wSD/libradioSoC52wSD.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/radioSoC/src"
    )
add_library(radioSoC52810 STATIC IMPORTED)
set_target_properties( radioSoC52810 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/radioSoC/cmakeBuild/libradioSoC52810.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/radioSoC/src"
        IMPORTED_LINK_INTERFACE_LIBRARIES nRF5x52810
    )
add_library(radioSoC52810NoSD STATIC IMPORTED)
set_target_properties( radioSoC52810NoSD
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/radioSoC/cmakeBuild/libradioSoC52810NoSD.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/radioSoC/src"
        IMPORTED_LINK_INTERFACE_LIBRARIES nRF5x52810
    )




add_library(nRF5x52 STATIC IMPORTED)
set_target_properties( nRF5x52
	PROPERTIES IMPORTED_LOCATION /home/bootch/git/nRF5x/Debug52/libnRF5x52.a
	INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/nRF5x/src"
	)
add_library(nRF5x52810 STATIC IMPORTED)
set_target_properties( nRF5x52810 
	PROPERTIES IMPORTED_LOCATION /home/bootch/git/nRF5x/cmakeBuild/libnRF5x52810.a
	INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/nRF5x/src"
	)


add_library(embeddedMath52 STATIC IMPORTED)
set_target_properties( embeddedMath52 PROPERTIES 
     IMPORTED_LOCATION /home/bootch/git/embeddedMath/Debug52/libembeddedMath52.a
     INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/embeddedMath/src" )



add_library(NRFDrivers STATIC IMPORTED)
set_target_properties( NRFDrivers PROPERTIES IMPORTED_LOCATION /home/bootch/git/libNRFDrivers/Debug/libNRFDrivers.a )


add_library(BLEProvisionee STATIC IMPORTED)
set_target_properties( BLEProvisionee 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/libBLEProvisionee/Debug/libBLEProvisionee.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/libBLEProvisionee/objects"
    )
# Same library but built for 52810
add_library(BLEProvisionee52810 STATIC IMPORTED)
set_target_properties( BLEProvisionee52810 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/libBLEProvisionee/Debug/libBLEProvisionee52810.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/libBLEProvisionee/objects"
    )
# Same library but built for 52810e emulated
add_library(BLEProvisionee52810e STATIC IMPORTED)
set_target_properties( BLEProvisionee52810e 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/libBLEProvisionee/Debug/libBLEProvisionee52810e.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/libBLEProvisionee/objects"
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
    nRF5x52
    embeddedMath52
    BLEProvisionee
    NRFDrivers
    )
# Same for 52810 except for BLEProvisionee !!!
target_link_libraries(firefly52810SDFanstel 
PRIVATE
    sleepSyncAgent
    radioSoC52wSD
    nRF5x52
    embeddedMath52
    BLEProvisionee52810
    NRFDrivers
    )

target_link_libraries(firefly52810NoSDFanstel 
PRIVATE
    sleepSyncAgent52xNoSD
    radioSoC52810NoSD
    nRF5x52810
    embeddedMath52
    )
    
# this target uses cmake build libraries
target_link_libraries(firefly52810e
PRIVATE
    sleepSyncAgent
    BLEProvisionee52810e
    radioSoC52810
    embeddedMath52
    NRFDrivers
    )
# nRF5x52810 is built into radioSoC
# NRFDrivers


target_link_libraries(firefly52810eNoSD
PRIVATE
    sleepSyncAgent52xNoSD
    radioSoC52810NoSD
    nRF5x52810
    embeddedMath52
    )
    
    
# declare transitive dependence
# target_link_libraries(radioSoC52810 nRF5x52810)

# TODO target firefly non-interactive
# TODO subdir radioSoC52SD sleepSync etc to insure they are built

   
# sources of executable
#     main which calls libraries
# SDK startup sources are property of target
# other SDK sources are built into libraries e.g. SEGGER_RTT in radioSoC
# and certain SDK modules in NRFDrivers
list(APPEND FireflySources
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
   )

target_sources(fireflyInteractive
   PUBLIC
      ${FireflySources}
   )
target_sources(firefly52810SDFanstel
   PUBLIC
      ${FireflySources}
   )
target_sources(firefly52810NoSDFanstel
   PUBLIC
      ${FireflySources}
   )
target_sources(firefly52810e
   PUBLIC
      ${FireflySources}
   )
target_sources(firefly52810eNoSD
   PUBLIC
      ${FireflySources}
   )

nRF5GenerateOtherArtifacts(fireflyInteractive)
nRF5GenerateOtherArtifacts(firefly52810SDFanstel)
nRF5GenerateOtherArtifacts(firefly52810NoSDFanstel)
nRF5GenerateOtherArtifacts(firefly52810e)
nRF5GenerateOtherArtifacts(firefly52810eNoSD)

# not generate FLASH_SOFTDEVICE target, already created by sniffer
# nRF5AddCustomTargets(fireflyInteractive)


