Files in this directory implement classes for a 'firefly' application.

All related to notion of "work"

Where:
- units work in synch, usually
- work is flashing an LED.
- voltage of power supply is managed via work

Units may flash at different intensities (according to their individual power supply)

Units may all flash at the same time.
Units may flash separately (but at a sync point) when they have excess power.
Units may fail to flash with others when unit has low power.

Units may flash out of sync (until the sleep sync algorithm achieves sync.)

Sync may be achieved and then lost, when units don't have enough power for radios to execute sleep sync algorithm.
