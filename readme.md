Digital, solar fireflies using Nordic radio modules, that blink in unison.  No batteries, sleep mostly.

Work in progress.

Background
-

Firefly insects synchronize their flashing.

There are analog electronic fireflies (communicating through their flashes.)  Using batteries.

Here, also electronic, but:

     digital
     communicate via radio
     power is solar power  
     
Much of the difficulty is:

    sleep synchronization: making units wake at the same time.
    managing a limited power budget.

    
Environment
-
I use:

     nRF52DK development board having SEGGER debug probe
     Ubuntu 16.04 LTS
     Eclipse Neon version
     Eclipse ARM GCC plugin
     Nordic NRF_SDK v12
    
See Also
-

The project derives from nRFRawProtocol which contains the original wireless protocol and main() for firefly.

The used libraries (also on Github) see Libraries below.
     

Building
-

Change the project's *Properties>C/C++ Build>Build Variables>NRF_SDK* variable to point to where you downloaded the Nordic NRF\_SDK.  
 
Choose a build configuration:  right mouse click on the project and choose *Properties>Build Configurations>Set Active>Foo*

Start the build:  click on the hammer icon, or choose *Project>Build All*

The project uses libraries and headers from other projects.  You might need to build those projects and copy the libraries and headers into this project.  If you are using a version of the NRF\_SDK other than v12, you might need to build the libraries on that base.

The library nrf5x must be built with LOGGING defined to build sniffer?

Sniffer
-

There are two apps here: firefly and a sniffer for the firefly protocol.
The build configurations do NOT configure LED's for each board using board.h.
LED's are configured in the call to LEDService.init() and different configurations are configured according to fireflyConfig.h

Debugging
-

Eclipse with the GNU ARM Plugin supports the nRF52DK and its Segger debug probe nicely.  Plug in your DK dev kit's USB and choose *Debug As>Debug Configurations>fireFly52*.

(You may want to create your own Debug Configuration.)

To see the log, open a terminal and run JLinkRTTClient, which you can download from Segger.

You should see a sequence of events scroll by.

Build Configurations
-

Eclipse manages the build (you edit Build and Debug Configurations, not Makefiles).

This is for two target embedded chips, the nRF52 having an ARM M4 mcu and the nRF51 having an ARM M0 mcu.  Many resources are named with 51 or 52 when they are specific to a target.

You can set a build configuration active.  Then you edit it in the project's Properties.  See nRFCounter project for more explanation.  That was the original project where I used Eclipse managed builds instead of hand managed Makefiles.

List of build configurations: (this list might be outdated, and the descriptions might differ from the build configuration)
 - Debug51aa for RedBear Nano
 - Debug51ac for Waveshare Core
 - Debug52 for NRF52DK
 - DebugUBlox for UBlox Nina
 - sniffer52 for using NRF52DK as a sniffer of the Firefly protocol, logging events to JLinkRTTClient
 All the build configurations are with debug symbols, with assertions enabled, and with no optimization

Libraries
-

Depends on libraries : 
    
    - sleepSyncAgent
    - radioSoC
    - embeddedMath
    - nRF5x
  
Libraries are built in versions for the architecture Arm M0 or M4, with suffix 51 or 52.
Only nRF5x is intended to be specific to brand of radio chip (Nordic.)
All are specified in the Eclipse build configuration with paths to your local copy.
Order of linking is important


Board configuration
-

Before you build, edit fireFlyConfig.h and uncomment the definition corresponding to your board (so that LED's work.)
Also uncomment the definition about power supply (to enable power management functions: if power is battery or USB, no power management.)


Multiprotocol
-

Multiprotocol: using two or more protocols (stacks.)

Firefly uses SleepSync which has its own proprietary protocol.  It does not depend on Nordics Softdevice.

Early versions are single protocol (used ONLY SleepSync protocol) and were incompatible with Softdevice.  
Latest version is compatible with Softdevice and could be multiprotocol.
The second protocol (Bluetooth BLE) can be used for control e.g. provisioning.

It requires:

       - versions of the libraries (nRF5X) that are compatbile with Softdevice.
       - building Firefly with -DSOFTDEVICE_PRESENT.
       - adding components/softdevice to include paths (early so that nrf_soc_nosd/nrf_nvic.h is not found)
       
To get those, build the libraries with 
