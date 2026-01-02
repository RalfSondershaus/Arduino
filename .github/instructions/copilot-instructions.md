# GitHub Copilot Instructions - Arduino Embedded C++ Project

## Architecture Overview

This is a safety-critical embedded C++ project for Arduino (Mega/Nano) and cross-platform development (Windows/Linux). The codebase uses an automotive-style layered architecture:

- **Src/Gen/** - Generic/reusable components organized by layer:
  - `Bsw/` - Basic Software (Arduino/Win platform abstraction)
  - `Hal/` - Hardware Abstraction Layer
  - `Rte/` - Runtime Environment (AUTOSAR-inspired communication layer with ports/interfaces)
  - `Dcc/` - DCC protocol decoder (model railroad digital control)
  - `Util/` - Embedded STL-like utilities (`util::basic_string`, `util::fix_queue`, `util::basic_string_view`)
  - `Test/` - googletest and Unity frameworks
- **Src/Prj/** - Project-specific code:
  - `App/` - Applications (Signal, DccSniffer, BlinkSample, FireFlicker)
  - `UnitTest/` - Unit tests organized by component
- **Build/** - Build system (GNU Make with multi-target support)

### Key Design Patterns

**RTE (Runtime Environment)**: AUTOSAR-inspired communication via typed ports. Components communicate through `signal_rte::get_*()` and `signal_rte::set_*()` functions defined in [Src/Gen/Rte](Src/Gen/Rte). Check [Rte.h](Src/Gen/Rte/Rte.h) for port APIs.

**Calibration Pattern**: Each component has `*_cal` namespace for configuration data, often stored in EEPROM (CVs). Example: `signal_cal::get_signal_aspect_for_idx()` in Signal app.

**Double Buffering**: DCC decoder uses double-buffered FIFOs for ISR-safe packet handling. See [Decoder.h](Src/Gen/Dcc/Decoder.h).

**ROM Storage**: Use `ROM_CONST_VAR` (mapped to `PROGMEM` on AVR, empty on x86) for flash storage of constants. Access with platform-specific macros.

## Coding Conventions (Critical)

Follow [CodingStyle.md](CodingStyle.md) strictly:

- **Naming**: `snake_case` for variables/functions, `PascalCase` for project classes, `snake_case` for generic `util::` classes
- **Constants**: `kPascalCase` for `constexpr`, `ALL_CAPS` for `#define`
- **Namespaces**: Use module prefixes (`signal::`, `dcc::`, `util::`)
- **Types**: `enum class` preferred, avoid `_t` suffix on global types (POSIX conflict)
- **No**: Dynamic memory, RTTI, exceptions, bitfields for hardware registers
- **Comments**: Doxygen in headers (what), implementation comments (how/why)

Example:
```cpp
namespace signal {
    static constexpr uint8 kMaxSignals = 8;
    void signal_init();  // snake_case function
    class SignalController {};  // PascalCase project class
}
namespace util {
    class basic_string {};  // snake_case generic class
}
```

## Build System

**Command**: `Build/build.bat <project> <target> <compiler> <bsw> <test_framework> <action>`

Parameters:
- `<project>`: `App/Signal`, `App/BlinkSample`, `UnitTest/Gen/Util/Ut_String`, etc.
- `<target>`: `mega`, `nano`, `nano4`, `uno`, `win32`, `win64`
- `<compiler>`: `avr_gcc`, `gcc`, `msvc`
- `<bsw>`: `arduino`, `win`, `none`
- `<test_framework>`: `googletest`, `unity`, `none`
- `<action>`: `all`, `rebuild`, `clean`, `run`, `download`

**Examples**:
```bash
# Build Signal app for Arduino Mega
Build/build.bat App/Signal mega avr_gcc arduino none rebuild

# Build and run unit tests on Windows
Build/build.bat UnitTest/Gen/Util/Ut_String win32 gcc win unity run

# Flash to Arduino
Build/build.bat App/Signal mega avr_gcc arduino none download
```

**VS Code Tasks**: Use predefined tasks (Ctrl+Shift+P → "Run Task") for common builds. Tasks use `${input:*}` variables for project/target selection.

**Makefile Structure**: Main [Makefile.gmk](Build/make/Makefile.gmk) includes:
- [Build/make/Gen/Platform/Makefile_Platform.gmk](Build/make/Gen/Platform/Makefile_Platform.gmk) - Target-specific flags
- [Build/make/Gen/Rules/Makefile_Rules.gmk](Build/make/Gen/Rules/Makefile_Rules.gmk) - Compiler rules
- [Build/make/Prj/\<project\>/Makefile_Prj.gmk](Build/make/Prj) - Project file lists

## Testing

**Unit Tests**: Located in `Src/Prj/UnitTest/Gen/` mirroring `Src/Gen/` structure. Use Unity framework (Arduino/embedded/Windows/Linux).

Test naming: `Ut_<Component>` (e.g., `Ut_String`, `Ut_Packet`). Use `unity_adapt.h` for Unity/googletest compatibility macros like `EXPECT_EQ`.

**Cross-Compilation Strategy**: Tests run on both target (Arduino) and host (Windows/Linux) using conditional compilation.
On Windows/Linux, use standard library `std::` in addition to `util::` library, primarily for container classes.
```cpp
#ifdef ARDUINO
#define CFG_TEST_WITH_STD CFG_OFF
#else
#define CFG_TEST_WITH_STD CFG_ON
#endif
```

## Platform Abstraction

**Compiler Macros**: Defined in [Src/Gen/Platform/\<arch\>/\<compiler\>/Compiler.h](Src/Gen/Platform):
- `ROM_CONST_VAR` → `PROGMEM` (AVR) or empty (x86)
- Access PROGMEM: Use Arduino `pgm_read_byte()` family

**Standard Types**: Use [Std_Types.h](Src/Gen/Platform) types: `uint8`, `uint16`, `uint32`, `sint8`, `boolean`.

**BSW Switching**: Code adapts via `#ifdef ARDUINO` for Arduino-specific APIs vs. Windows emulation in [Src/Gen/Bsw/Win/](Src/Gen/Bsw/Win).

## Signal Application Specifics

The flagship app ([Src/Prj/App/Signal](Src/Prj/App/Signal)) controls model railroad signals via DCC or analog inputs (buttons with resistor ladder). Key components:

- **InputClassifier**: Converts ADC values to discrete aspect commands using hysteresis
- **Signal**: State machine managing LED intensity ramps and aspect transitions
- **LedRouter**: PWM output to signal LEDs
- **DccDecoder**: Filters DCC accessory packets for configured addresses
- **AsciiCom**: Serial terminal commands (`SET_SIGNAL`, `GET_CV`, `MON_START`) for configuration

**CV (Configuration Variables)**: EEPROM-stored config mimicking DCC CV standard. See [Src/Prj/App/Signal/Doc/Readme.md](Src/Prj/App/Signal/Doc/Readme.md#list-of-cvs) for CV map.

**Built-in Signals**: Three predefined German railway signals (Ausfahrsignal, Blocksignal, Einfahrsignal) with aspect-to-LED mappings in [Cal/CalM.cpp](Src/Prj/App/Signal/Cal/CalM.cpp).

## Common Pitfalls

- **Bitfield Ordering**: Compiler-dependent, use bitmasks instead for hardware registers
- **PROGMEM Access**: Must use `pgm_read_*()` on AVR; direct access causes crashes
- **RTE Ports**: Always check port validity before use (`signal_rte::isValid()`)
- **Fixed-Size Containers**: `util::basic_string<N>`, `util::fix_queue<T,N>` for stack allocation
- **Test Framework Differences**: Unity uses `TEST_ASSERT_*`, googletest uses `EXPECT_*`/`ASSERT_*`

## Documentation

- [README.md](README.md) - Project overview
- [CodingStyle.md](CodingStyle.md) - Mandatory style guide
- [Install.txt](Install.txt) - Toolchain setup (GnuWin32, AVR-GCC)
- [Src/Prj/App/Signal/Doc/Readme.md](Src/Prj/App/Signal/Doc/Readme.md) - Signal app usage
- Doxygen: Run `doxygen Doxyfile` → outputs to [Build/Doc/html/](Build/Doc/html)

## Quick Start for New Components

1. **Choose Layer**: Application code → `Src/Prj/App/`, reusable → `Src/Gen/<layer>/`
2. **Create Makefile**: Add `Build/make/Prj/<path>/Makefile_Prj.gmk` with `FILES_PRJ` list
3. **Add RTE Ports**: If inter-component communication needed, extend [Rte_Types_Prj.h](Src/Gen/Rte/Rte_Types_Prj.h)
4. **Write Tests**: Mirror structure in `Src/Prj/UnitTest/` with corresponding makefile
5. **Use Namespace**: Match component name (`signal::` for Signal app)
6. **Follow Style**: `snake_case` functions, `PascalCase` classes (project-specific), Doxygen headers

## Debugging

- **Arduino Serial**: 115200 baud, use `MON_START` commands to monitor RTE data
- **VS Code**: Launch configs in [.vscode/launch.json](.vscode/launch.json) for Windows debugging
- **avrdude**: Use `-v` for verbose flashing output if downloads fail

# GitHub Copilot Instructions: MISRA C++ Analysis for Embedded Systems

## 🎯 Objective

This repository contains C++ code intended for use in embedded systems. GitHub Copilot shall assist
by identifying and commenting on potential violations of the MISRA C++ guidelines (e.g., 2008 or 
2023), especially those relevant to resource-constrained microcontrollers and real-time 
applications.

## ✅ Scope of Analysis

Copilot should focus on:

- **MISRA C++ rules** related to:
  - Type safety and implicit conversions
  - Use of dynamic memory (not allowed)
  - Exception handling (banned)
  - Virtual functions and inheritance
  - Initialization and object lifetime
  - Use of STL (forbidden and replaced by custom implementations)
  - Side effects in expressions
  - Control flow clarity (e.g., no `goto`, no recursion)
  - Bitwise operations and fixed-width types

- **Embedded-specific concerns**:
  - Deterministic behavior
  - Memory footprint and stack usage
  - Portability across compilers and architectures
  - Interrupt safety and reentrancy
  - Use of volatile and hardware registers
  - Avoidance of heap allocation and RTTI

## 🧠 Copilot Behavior Guidelines

- Prefer **static analysis-style comments** over code changes unless explicitly requested.
- Flag **non-compliant constructs** with inline comments using the format:

  ```cpp
  int x = 0; // MISRA C++ Rule 6-4-1: Avoid implicit conversions from signed to unsigned
