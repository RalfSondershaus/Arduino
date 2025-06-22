# Arduino
This repo contains a collection of Arduino projects:

- **BlinkSample** - a Hello World program: let LED at pin 13 (onboard LED) blink.
- **DccSniffer** - Sniff DCC protocol and output via serial interface [Src/Prj/App/DccSniffer/Doc/Readme.md](Src/Prj/App/DccSniffer/Doc/Readme.md)
- **Signal** - Control signals via buttons or DCC, see [Src/Prj/App/Signal/Doc/Readme.md](Src/Prj/App/Signal/Doc/Readme.md)
- **FireFlicker** - Simulate a campfire with three LEDs (or bulbs)

The samples are based on generic code which compiles on different platforms including Arduino Mega and Nano, but also Windows or Linux.
Note that for DCC, you need additional hardware.

The code structure at top level is organized as follows:
- **Build** - everything to build a project. We use make, so this folder contains the makefiles.
- **Src** - the source code
- **Docker** - Docker image configuration (experimental)
- **.vscode** - JSON files for Visual Studio Code configuration
- **.github/workflows** - GitHub workflow files
