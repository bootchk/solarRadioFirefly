Sniffer app.
Runs on NRF52DK.
Logs  using RTT to a display.
Start debugger in Eclipse on Sniffer, and start JLinkRTTClient in a terminal, and they will connect.


Requires a Nordic file app_util_platform.cpp.
It is copied to ../nrf52/ and renamed to .cpp
because starting with NRF SDK 12.2 Segger RTT uses it.

Logging is implemented in libradioSoC52Log.a.  No SDK SEGGER_RTT_<foo>.c files are in this project.