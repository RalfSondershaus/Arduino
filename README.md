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

## License

This project is licensed under the Apache License, Version 2.0.

You may use this software in commercial and non-commercial projects, modify it, and redistribute it under the terms of the Apache-2.0 license. A copy of the license is provided in the `LICENSE` file in this repository.

Source files include SPDX identifiers for machine-readable license information.

## Third-Party Software

This repository integrates third-party components essential for testing and operation:

- **FreeRTOS**  
  Version: v202112.00  
  Location: `Src/Gen/Os/FreeRTOS/v202112.00/`  
  License: MIT  
  Homepage: [FreeRTOS](https://www.freertos.org/)

- **Arduino Core**  
  Version: AVR 1.8.6  
  Location: `Src/Gen/Bsw/Arduino/avr/1.8.6/`  
  License: LGPL v2.1  
  Homepage: [Arduino Core AVR](https://github.com/arduino/ArduinoCore-avr)

- **GoogleTest** (Unit Testing Framework)  
  Version: 1.12.1
  Location: `Src/Gen/Test/googletest/`  
  License: BSD 3-Clause  
  Homepage: [GoogleTest](https://github.com/google/googletest)

- **Unity** (Unit Testing Framework for C) 
  Version: 2.6.0 (see `unity.h`)
  Location: `Src/Gen/Test/unity/`  
  License: MIT  
  Homepage: [Unity](https://github.com/ThrowTheSwitch/Unity)

Each third-party component is stored in a dedicated folder with its license information.
Changes to original third-party code, if any, are documented in an accompanying `MODIFICATIONS.md` inside the respective directory.