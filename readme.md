Digital,solar fireflies.  Nordic modules that blink in unison.  No batteries, sleep mostly.


Background
-

Firefly insects synchronize their flashing.

There are analog electronics fireflies (communicating through their flashes.)  Using batteries.

Here, also electronic, but:

     digital
     communicate via radio.
     power is solar power  
     
Much of the difficulty is in sleep synchronization: making the units wake at the same time.
Also, managing limited power budget.

    
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

Uses libraries (also on Github):

     sleepSyncAgent
     nRF5x
     

Building
-

Change the project's *Properties>C/C++ Build>Build Variables>NRF_SDK* variable to point to where you downloaded the Nordic NRF\_SDK.  
 
Choose a build configuration:  right mouse click on the project and choose *Properties>Build Configurations>Set Active>Foo*

Start the build:  click on the hammer icon, or choose *Project>Build All*

The project uses libraries and headers from nother projects.  You might need to build that project and copy the library and header into this project.  If you are using a version of the NRF\_SDK other than v12, you might need to build the libraries on that base.

Debugging
-

Eclipse with the GNU ARM Plugin supports the nRF52DK and its Segger debug probe nicely.  Plug in your DK dev kit's USB and choose *Debug As>Debug Configurations>fireFly52*.

(You may want to create your own Debug Configuration.)

To see the log, open a terminal and run JLinkRTTClient, which you can download from Segger.

You should see a sequence of events scroll by.

Reconfiguring
-

Eclipse manages the build (you edit Build and Debug Configurations, not Makefiles).

This is for two target embedded chips, the nRF52 having an ARM M4 mcu and the nRF51 having an ARM M0 mcu.  Many resources are named with 51 or 52 when they are specific to a target.

You can set a build configuration active.  Then you edit it in the project's Properties.  See nRFCounter project for more explanation.  That was the original project where I used Eclipse managed builds instead of hand managed Makefiles.