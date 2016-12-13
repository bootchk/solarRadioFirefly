A minimal Eclipse project for testing libraries for two targets in the nRF5x family of embedded radios (ARM architecture.)

If you don't use Eclipse and nRF5x, it's not interesting.  The value is in the Eclipse project's build and debug configurations, for learning or modifying.

As shipped, main() just tests another library.


Motivation
-

    avoid hand-built Makefiles of Nordic example projects.
    divide projects into statically linked libraries
    use C++
    
Environment
-
I use:

     nRF52DK development board having SEGGER debug probe
     Ubuntu 16.04 LTS
     Eclipse Neon version
     Eclipse ARM GCC plugin
    
See Also
-

I started here:

https://bitknitting.wordpress.com/2015/09/06/using-eclipse-for-nrf51-development-getting-off-makefile-and-into-managed-make/

This project's configuration follows that tutorial, but:

    updated to have multiple build configurations:

         for multiple targets: nRF51 and nRF52.
         linking in target specific libraries
         
    changed from C to C++


Building
-

Change the project's *Properties>C/C++ Build>Build Variables>NRF_SDK* variable to point to where you downloaded the Nordic NRF\_SDK.  
 
Choose a build configuration:  right mouse click on the project and choose *Properties>Build Configurations>Set Active>Foo*

Start the build:  click on the hammer icon, or choose *Project>Build All*

The project uses a library and its header from another project (nRFCounter).  You might need to build that project and copy the library and header into this project.

If you are using a version of the NRF\_SDK other than v12, you might need to refresh files that were copied from the NRF_SDK (see below.)

Debugging
-

Eclipse with the GNU ARM Plugin supports the nRF52DK and its Segger debug probe nicely.  Plug in your DK dev kit's USB and choose *Debug As>Debug Configurations>nRFCounter*.

(You may want to create your own Debug Configuration.)

To see the log, open a terminal and run JLinkRTTClient, which you can download from Segger.

You should see a sequence of times scroll by.

Reconfiguring
-

Eclipse manages the build (you edit Build and Debug Configurations, not Makefiles).

This is for two target embedded chips, the nRF52 having an ARM M4 mcu and the nRF51 having an ARM M0 mcu.  Many resources are named with 51 or 52 when they are specific to a target.

You can set a build configuration active.  Then you edit it in the project's Properties.

*Properties>C/C++ General>Paths and Symbols>Symbols* defines preprocessor symbol NRF51 or NRF52 which conditionally compiles code (mainly in Nordic provided files.)

Other configuration discussed below.


Files and Directories
=

This explains how various directories support multiple targets, and how a build configuration chooses target specific files.

src directory
-

Contains target independendent source.

main.cpp minimally tests a library.  (The only target dependence in main.cpp is "include nrf.h" from the NRF_SDK, needed for the sleep() function, which should itself be in a library.)

The library is minimal. It simply wraps the "RTC" counter/compare device of the target.  nRFCounter.h is its target independent header.  The library is lowest common denominator: only supports features/peripheral/devices found on both targets.  libnRFCounter52.a or 51.a are the libraries, statically compiled for a target (specific to an mcu architecture.)

SEGGER_RTT... files are for logging using a SEGGER debug probe.  Again, copied from the SDK but should be soft linked.


nrf51 and nrf52 directories
-
Contains target specific source.

An entry in *Properties>C/C++ General>Paths and Symbols>Source Location* chooses one.

gcc\_startup\_nrf52.S is assembler that defines interrupt vectors and handlers, especially the Reset\_Handler vector and handler, which is called at reset/boot time.  Again, copied from the NRF_SDK v12, but it should probably be soft linked in the project.  (Name changed: for some reason, the capital .S signfies to Eclipse that it should get compiled into the project?)

system\_nrf52.cpp defines patches for the chip.  (I don't really understand how it it is called or works.)  Again, copied from NRF_SDK, name changed, and should be linked instead.

libraries
-

The project top level contains target specific static libraries libnRFCounter52.a and ...51.a.  

*Properties>C/C++ General>Settings>Cross ARM C++ Linker>Libraries>Libraries* choses one.


linkerScripts
-
Contains target and chip revision specific linker scripts (see link above to a tutorial.)  ...52.ld and ...51.d.  Again, copied from the SDK but should be soft linked.

*Properties>C/C++ General>Settings>Cross ARM C++ Linker>General>ScriptFiles* choses one.

You might customize the linker scripts to change how memory is allocated.

An additional file from the NRF\_SDK is included by the .ld script.  It is found via a path in the build configuration.  *Properties>C/C++ General>Settings>Cross ARM C++ Linker>Libraries>Library search paths* specifies "${NRF_SDK}/components/toolchain/gcc", the location of the additional included linker script.

Binaries, Debug51, Debug52
-
These directories are created by Eclipse build system.  They contain artifacts: target specific intermediate files and executables.

*Properties>C/C++ General>Settings>Build Artifacts>Artifact name* specializes the name of each artifact by target (51 or 52).

Later, a Debug Configuration's *C/C++ Executable* chooses a target specific artifact by name, e.g. Debug52/testnRFCounter52.elf

