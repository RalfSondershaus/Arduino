# GitHub Copilot Instructions - Arduino Embedded C++ Project

## Architecture Overview

This is a safety-critical embedded C++ project for Arduino (Mega/Nano) and cross-platform development (Windows/Linux). The codebase uses an automotive-style layered architecture:

- **Src/Gen/** - Generic/reusable components organized by layer:
  - `Bsw/` - Basic Software (Arduino/Win platform abstraction)
  - `Hal/` - Hardware Abstraction Layer to abstract `Arduino.h` APIs to hal::adc_*, hal::gpio_*, hal::timer_*, etc.
  - `Rte/` - Runtime Environment (AUTOSAR-inspired communication layer with ports/interfaces)
  - `Dcc/` - DCC protocol decoder (model railroad digital control)
  - `Util/` - Embedded STL-like utilities (`util::basic_string`, `util::fix_queue`, `util::basic_string_view`)
  - `Test/` - googletest and Unity frameworks
- **Src/Prj/** - Project-specific code:
  - `App/` - Applications (Signal, DccSniffer, BlinkSample, FireFlicker)
  - `UnitTest/` - Unit tests mirroring Gen/ and App/ structure
- **Build/** - Build system (GNU Make with multi-target support)
- **.github/** - GitHub CoPilot and GitHub workflow configuration and instructions

### Key Design Patterns

**RTE (Runtime Environment)**: AUTOSAR-inspired communication via typed ports. Components communicate through `rte::ifc_*::read()` and `rte::ifc_*::write()` functions or convenience wrappers like `rte::set_cv()` and `rte::get_cv()`. Ports are defined in [Src/Gen/Rte/Rte_Types_Prj.h](Src/Gen/Rte/Rte_Types_Prj.h). The RTE provides runtime monitoring via `rte::getNrPorts()` and `rte::getPortData()` for debugging.

**Calibration Pattern**: Configuration variables (CVs) stored in EEPROM follow DCC CV convention. Each component has a `cal::` namespace (e.g., `cal::cv::kSignalIDBase`). CV IDs are defined in [Cal/CalM_Types.h](Src/Prj/App/Signal/Cal/CalM_Types.h), accessed via `rte::set_cv()` and `rte::get_cv()`.

**Double Buffering**: DCC decoder uses double-buffered FIFOs (`util::fix_queue`) for ISR-safe packet handling. See [Decoder.h](Src/Gen/Dcc/Decoder.h) for the ping-pong buffer pattern between interrupt and main loop contexts.

**DCC Protocol (NMRA Standards)**: Implementation follows NMRA S-9.1 (electrical/timing) and S-9.2 (packet format):
- **Bit Encoding (S-9.1)**: Bipolar square wave, bits encoded by timing between zero-crossings
  - **"1" bit**: 
    - Command station transmits: 55-61 µs per half-bit (nominal 58 µs)
    - Decoder accepts: 52-64 µs per half-bit
    - Asymmetry tolerance: ≤6 µs difference between halves
    - Total: ~116 µs
  - **"0" bit**: 
    - Command station transmits: 95-9,900 µs per half-bit
    - Decoder accepts: 90-10,000 µs per half-bit
    - Total: ≥200 µs, max 12,000 µs
  - **Constants**: See [BitExtractor.h](Src/Gen/Dcc/BitExtractor.h): `kPartTimeShortMin/Max` (52-64 µs), `kPartTimeLongMin/Max` (90-10,000 µs)
- **Packet Structure (S-9.2)**:
  - General: `{preamble} 0 ADDRESS 0 INSTRUCTION 0 XOR 1`
  - Preamble: ≥10 consecutive "1" bits (typically 16)
  - Separator: "0" bit after preamble and after each data byte
  - Data bytes: Address + instruction + data (variable length)
  - Checksum: XOR of all preceding bytes
  - End: "1" bit separator
- **Accessory Decoder Addressing (S-9.2.1)** - Relevant for Signal app:
  - **Address ranges**: 128-191 (basic/extended accessory), 253-254 (advanced), 255 (idle)
  - **9-bit basic**: Primary address (A7-A0) + output pair selection
  - **11-bit extended**: Combines primary + secondary address for more outputs
  - **Binary State Control**: Up to 32,767 independent states (multi-aspect signals)
    - Short form (1-127 states): `11011101 0 DLLLLLLL` (D=state, L=state number)
    - Long form (1-32,767): `11000000 0 DLLLLLLL 0 HHHHHHHH` (15-bit state)
    - States are NOT refreshed - decoder must store values (NVRAM recommended)
  - **Function Groups**: F13-F20, F21-F28, F29-F36 for extended features
  - **Implementation**: See [Filter.h](Src/Gen/Dcc/Filter.h) for packet filtering by address
- **Timing Analysis**: 3-byte packet ~6.5 ms (16-bit preamble) = ~150 packets/sec max
- **State Machine**: `bit_extractor` (half-bit timing → bits) → `packet_extractor` (bits → packets) → `decoder` (double-buffered FIFO)
- **Key Implementation Notes**:
  - Decoder must tolerate jitter and asymmetry within S-9.1 ranges
  - ISR measures time between pin change interrupts (zero-crossings)
  - Real-world noise: decoder must achieve ≥95% decode probability with noise ≤¼ signal amplitude
  - **ISR Performance Critical**: ISR must complete in <26 µs (half of shortest valid half-bit: 52 µs). Keep ISR minimal - only timestamp capture and state update. Defer packet processing to main loop via double-buffered FIFO.

Reference: https://www.nmra.org/index-nmra-standards-and-recommended-practices (S-9.1: electrical, S-9.2: packets)

**ROM Storage**: Use `ROM_CONST_VAR` macro (mapped to `PROGMEM` on AVR, empty on x86) for flash storage of constants. Access with `ROM_READ_PTR()`, `ROM_READ_STRING()`, `ROM_READ_STRUCT()` macros defined in [Compiler.h](Src/Gen/Platform).

## Coding Conventions (Critical)

Follow [CodingStyle.md](CodingStyle.md) strictly:

- **Naming**: 
  - `snake_case` for variables/functions/generic `util::` classes
  - `PascalCase` for project-specific classes (e.g., `MotorController`)
  - Module prefixes for functions (e.g., `adc_init()`, `signal_process()`)
- **Constants**: `kPascalCase` for `constexpr`, `ALL_CAPS` for `#define`
- **Types**: `enum class` preferred, avoid `_t` suffix on global types (POSIX conflict), use `_type` suffix for member types
- **Namespaces**: Use layer prefixes (`signal::`, `dcc::`, `util::`, `rte::`, `cal::`)
- **No**: Dynamic memory (`new`/`delete`), RTTI, exceptions, bitfields for hardware registers, STL (use `util::` instead)
- **Comments**: Doxygen in headers (what), implementation comments (how/why)

Do never include `Arduino.h` directly in `Gen/` or `Prj/App/`; always go through `Hal/`. This 
ensures platform abstraction and testability with stub implementations.

Example:
```cpp
namespace signal {
    static constexpr uint8 kMaxSignals = 8;  // constexpr constant
    void signal_init();                      // snake_case function with prefix
    class SignalController {};               // PascalCase project class
}
namespace util {
    class basic_string {};                   // snake_case generic class
}
```

**Critical Don'ts**: Avoid Hungarian notation (`uc_`, `un_`), avoid cryptic abbreviations (`nr_` → use `num_` or `count`), no type info in names.

## Build System

**Command**: `Build/build.bat <project> <target> <compiler> <bsw> <test_framework> <action>`

Parameters:
- `<project>`: Relative path from `Src/Prj/`, e.g., `App/Signal`, `App/BlinkSample`, `UnitTest/Gen/Util/Ut_String`
- `<target>`: `mega`, `nano`, `nano4`, `uno`, `win32`, `win64`
- `<compiler>`: `avr_gcc`, `gcc`, `msvc`
- `<bsw>`: `arduino` (AVR), `win` (Windows/Linux), `none`
- `<test_framework>`: `googletest`, `unity`, `none`
- `<action>`: `all`, `rebuild`, `clean`, `run`, `download` (flash to Arduino)

**Examples**:
```bash
# Build Signal app for Arduino Mega
Build/build.bat App/Signal mega avr_gcc arduino none rebuild

# Build and run unit tests on Windows
Build/build.bat UnitTest/Gen/Util/Ut_String win32 gcc win unity run

# Flash to Arduino Mega
Build/build.bat App/Signal mega avr_gcc arduino none download
```

**VS Code Tasks**: Use predefined tasks (Ctrl+Shift+P → "Run Task") for common builds. Task definitions in [.vscode/tasks.json](.vscode/tasks.json) use `${input:*}` variables for interactive project/target selection.

**Makefile Structure**: 
- Main: [Build/make/Makefile.gmk](Build/make/Makefile.gmk)
- Platform config: [Build/make/Gen/Platform/Makefile_Platform.gmk](Build/make/Gen/Platform/Makefile_Platform.gmk) - target-specific compiler flags
- Compiler rules: [Build/make/Gen/Rules/Makefile_Rules.gmk](Build/make/Gen/Rules/Makefile_Rules.gmk) - build recipes
- Project files: [Build/make/Prj/\<project\>/Makefile_Prj.gmk](Build/make/Prj) - `FILES_PRJ` lists source files

**Build Artifacts**:
- Binaries: `Build/Bin/<project>/` (.elf, .hex, .map)
- Intermediates: `Build/Tmp/<project>/` (.o, .d dependency files)

**Toolchain Setup**: Install GnuWin32 (make), AVR-GCC, avrdude. See [Install.txt](Install.txt). Paths with spaces require network drive mapping.

## Testing

**Unit Tests**: Located in `Src/Prj/UnitTest/` mirroring `Src/Gen/` and `Src/Prj/App/` structure. Use Unity framework for cross-platform testing (Arduino/Windows/Linux).

Test naming: `Ut_<Component>` (e.g., `Ut_String`, `Ut_Packet`, `Ut_Signal`). 

**Test Adapter Pattern**: Use `unity_adapt.h` for Unity/googletest API compatibility. Provides macros like `EXPECT_EQ`, `EXPECT_TRUE` that map to Unity's `TEST_ASSERT_*` macros.

Example:
```cpp
#include <unity_adapt.h>

TEST(String, Append) {
    util::basic_string<10, char> str;
    str.append("test");
    EXPECT_EQ(str.size(), 4);  // Maps to TEST_ASSERT_EQUAL
}
```

**Stub Functions**: Arduino-specific functions (e.g., `millis()`, `micros()`) are encapsulated in 
the HAL, which provides stubs in addition to real implementations. Use stub return value variables
for testing if required.

In order to use the stubs, include the HAL headers in your test files:
```cpp
#include <Hal/Timer.h>
```

and add the stub implementation files to your test makefile, e.g., stub for `Hal/Timer` in 
`Build/make/Prj/UnitTest/Prj/Ut_Signal/Makefile_Prj.gmk`:
```
# Files
FILES_PRJ = $(PATH_SRC_PRJ_PROJECT)/Test            \
            $(PATH_SRC_HAL)/Stub/Timer/Hal/Timer

# Includes
C_INCLUDES_PRJ := $(C_INCLUDES_PRJ)                \
                  -I$(PATH_SRC_HAL)/Stub/Timer
```

**Cross-Compilation Strategy**: Tests run on both target (Arduino) and host (Windows/Linux) using conditional compilation:
```cpp
#ifdef ARDUINO
#define CFG_TEST_WITH_STD CFG_OFF  // Use only util::
#else
#define CFG_TEST_WITH_STD CFG_ON   // Can use std:: for comparison
#endif
```

**Running Tests**:
```bash
# Windows unit test
Build/build.bat UnitTest/Gen/Util/Ut_String win32 gcc win unity run

# Arduino unit test (flash to board)
Build/build.bat UnitTest/Gen/Util/Ut_String mega avr_gcc arduino unity download
```

## Platform Abstraction

**Compiler Macros**: Defined in [Src/Gen/Platform/\<arch\>/\<compiler\>/Compiler.h](Src/Gen/Platform):
- `ROM_CONST_VAR` → `PROGMEM` (AVR) or empty (x86)
- Access PROGMEM: Use Arduino `pgm_read_byte()` family

**Standard Types**: Use [Std_Types.h](Src/Gen/Platform) types: `uint8`, `uint16`, `uint32`, `sint8`, `boolean`.

**BSW Switching**: Code adapts via `#ifdef ARDUINO` for Arduino-specific APIs vs. Windows emulation in [Src/Gen/Bsw/Win/](Src/Gen/Bsw/Win).

## Signal Application Specifics

The flagship app ([Src/Prj/App/Signal](Src/Prj/App/Signal)) controls model railroad signals via DCC or analog inputs. **Functions as a DCC accessory decoder** supporting multi-aspect signals.

Key components:

- **InputClassifier**: Converts ADC values to discrete aspect commands using hysteresis (`util::Classifier`)
- **Signal**: State machine managing LED intensity ramps (`util::Ramp`) and aspect transitions
- **LedRouter**: PWM output to signal LEDs via RTE ports
- **DccDecoder**: Filters DCC accessory packets (address range 128-191) for configured addresses
  - Supports binary state control for multi-aspect signals
  - Packet filtering in [Filter.h](Src/Gen/Dcc/Filter.h)
  - No automatic refresh - states stored in EEPROM via CVs
- **AsciiCom**: Serial terminal commands for runtime configuration and monitoring

**CV (Configuration Variables)**: EEPROM-stored config mimicking DCC CV standard. Base addresses in [Cal/CalM_Types.h](Src/Prj/App/Signal/Cal/CalM_Types.h):
- `cal::cv::kSignalIDBase` (42-49): Signal type ID per position
- `cal::cv::kSignalFirstOutputBase` (50-57): Output pin config (onboard/external + pin number)
- `cal::cv::kSignalInputBase` (58-65): Input config (ADC/DCC/DIG + pin)
- `cal::cv::kSignalOutputConfigBase` (74-81): Step size and inverse order

**Built-in Signals**: Three predefined German railway signals in [Cal/CalM.cpp](Src/Prj/App/Signal/Cal/CalM.cpp):
- Signal ID 0: Ausfahrsignal (main exit signal)
- Signal ID 1: Blocksignal (block signal)
- Signal ID 2: Einfahrsignal (entry signal)

User-defined signals: IDs 128-129 configurable via CVs.

**Serial Commands** (115200 baud):
- `SET_SIGNAL idx id [ONB,EXT] pin step [ADC,DIG,DCC] input` - Configure signal
- `GET_SIGNAL idx` - Read signal config
- `SET_CV cv_id value` / `GET_CV cv_id` - Direct CV access
- `MON_START cycle_ms ifc_name [first_idx nr_idx]` - Monitor RTE port data
- `MON_LIST` - List available RTE ports
- `ETO_SET_SIGNAL idx aspect [dim_time]` - Emergency takeover
- `INIT` - Reset to defaults

See [AsciiCom.h](Src/Prj/App/Signal/Com/AsciiCom.h) for complete command reference.

## Common Pitfalls

- **PROGMEM Access**: On AVR, constants marked `ROM_CONST_VAR` require `ROM_READ_PTR()`, `ROM_READ_STRING()`, `ROM_READ_STRUCT()` macros. Direct access causes crashes. Example in [AsciiCom.cpp](Src/Prj/App/Signal/Com/AsciiCom.cpp).
- **RTE Port Validity**: Some ports are compile-time optional. Always check existence before use.
- **Fixed-Size Containers**: `util::basic_string<N>`, `util::fix_queue<T,N>` require compile-time size. Exceeding capacity silently truncates or fails.
- **Test Framework Differences**: Unity uses `TEST_ASSERT_*`, googletest uses `EXPECT_*`/`ASSERT_*`. Use `unity_adapt.h` for portability.
- **Bitfield Ordering**: Compiler-dependent, avoid for hardware registers. Use explicit bitmasks instead.
- **ISR Safety**: Use `util::fix_queue` for ISR-to-main communication (DCC packets). Avoid complex operations in ISRs.
- **CV ID Ranges**: Built-in signals 0-127, user-defined 128+. Signal positions 0-7 (Arduino Mega). Validate indices.
- **Step Size Logic**: Negative values (-1, -2) indicate inverse output pin order. Zero is invalid.

## Documentation

- [README.md](README.md) - Project overview
- [CodingStyle.md](CodingStyle.md) - Mandatory style guide
- [Install.txt](Install.txt) - Toolchain setup (GnuWin32, AVR-GCC)
- [Src/Prj/App/Signal/Doc/Readme.md](Src/Prj/App/Signal/Doc/Readme.md) - Signal app usage
- Doxygen: Run `doxygen Doxyfile` → outputs to [Build/Doc/html/](Build/Doc/html)

## Quick Start for New Components

1. **Choose Layer**: 
   - Application code → `Src/Prj/App/<ComponentName>/`
   - Reusable generic → `Src/Gen/<layer>/<ComponentName>/` (layer = Util, Hal, Dcc, etc.)
2. **Create Makefile**: Add `Build/make/Prj/<path>/Makefile_Prj.gmk` with `FILES_PRJ` variable listing source files
3. **Add RTE Ports** (if inter-component communication needed):
   - Define port in [Rte_Types_Prj.h](Src/Gen/Rte/Rte_Types_Prj.h) using `RTE_DEF_*` macros
   - Implement port in [Rte_Cfg_Prj.h](Src/Gen/Rte/Rte_Cfg_Prj.h)
4. **Write Tests**: Mirror structure in `Src/Prj/UnitTest/` with matching makefile
5. **Use Namespace**: Match component name (`signal::`, `dcc::`, `util::`)
6. **Follow Style**: 
   - `snake_case` functions with module prefix (e.g., `signal_init()`)
   - `PascalCase` for project classes
   - `kPascalCase` for constants
   - Doxygen in headers

Example project structure:
```
Src/Prj/App/MyComponent/
├── MyComponent.h          # Public interface with Doxygen
├── MyComponent.cpp        # Implementation
├── Cal/
│   ├── CalM_Types.h       # CV definitions
│   └── CalM.cpp           # Calibration data
Build/make/Prj/App/MyComponent/
└── Makefile_Prj.gmk       # FILES_PRJ = MyComponent.cpp Cal/CalM.cpp
```

## Debugging

- **Arduino Serial**: 115200 baud, 8N1. Use `MON_START cycle_ms ifc_name` to monitor RTE data in real-time
- **HTerm Settings**: Send on enter = `CR`, Newline at `CR+LF`, disable "Show newline characters"
- **VS Code**: Launch configs in [.vscode/launch.json](.vscode/launch.json) for Windows debugging with GDB/MSVC
- **avrdude**: Use `-v` for verbose flashing output if downloads fail. Common issue: wrong COM port or driver
- **Memory Analysis**: 
  - `objdump -s -j <section> <file>.elf` - Inspect .data, .bss, .text sections
  - `nm --size-sort --radix=d <file>.elf | c++filt` - Symbol sizes (demangle C++)
- **Performance Testing**: Unit tests `Ut_Signal_Performance`, `Ut_Sstream_Performance` measure cycle counts

## Key Utilities

The `util::` namespace provides embedded-friendly STL alternatives:

- **Strings**: `util::basic_string<N, CharT>` - Fixed-capacity string, no dynamic allocation
- **String Views**: `util::basic_string_view<CharT>` - Non-owning string reference
- **Streams**: `util::basic_istringstream<N, CharT>` - String parsing with `>>` operator
- **Containers**: `util::fix_queue<T, N>`, `util::fix_deque<T, N>`, `util::array<T, N>`
- **Algorithms**: `util::copy()`, `util::fill()`, `util::min()`, `util::max()`
- **Bits**: `util::bits::test()`, `util::bits::set()`, `util::bits::clear()`
- **Ramps**: `util::Ramp` - Linear interpolation for smooth transitions
- **Classifiers**: `util::Classifier` - Hysteresis-based input classification

All utilities are stack-allocated, deterministic, and work without heap.


---

# MISRA C++ Analysis Guidelines

## 🎯 Objective

This repository contains C++ code for safety-critical embedded systems. GitHub Copilot should identify potential violations of MISRA C++ guidelines (2008/2023), especially those relevant to resource-constrained microcontrollers and real-time applications.

## ✅ Scope of Analysis

Focus on MISRA C++ rules related to:

- **Type safety**: Implicit conversions, fixed-width types (`uint8`, `uint16`, `sint16`)
- **Memory**: No dynamic allocation (`new`/`delete` banned), no heap usage
- **Exceptions**: Banned (compiler flag `-fno-exceptions`)
- **Virtual functions**: Minimize due to vtable overhead
- **Initialization**: Always initialize variables, avoid uninitialized members
- **STL**: Forbidden (use `util::` alternatives)
- **Side effects**: Avoid in expressions, single responsibility per statement
- **Control flow**: No `goto`, no recursion (stack depth critical)
- **Bitwise operations**: Use explicit bitmasks, not bitfields
- **Embedded concerns**: ISR safety, volatile usage, deterministic behavior, stack usage

## 🧠 Copilot Behavior Guidelines

- Prefer **static analysis-style comments** over code changes unless explicitly requested
- Flag **non-compliant constructs** with inline comments:

  ```cpp
  int x = y + z++; // MISRA C++ Rule 5-2-10: Avoid side effects in expressions (++)
  ```

- Suggest **compliant alternatives** when violations are found
- Prioritize **critical violations** (memory safety, undefined behavior) over style issues
- Respect **project deviations**: Some MISRA rules are intentionally violated (document with `// MISRA Deviation: <reason>`)

## 🔍 Project-Specific MISRA Compliance

**Compliant patterns** in this codebase:
- Fixed-width types (`uint8`, `uint16`) instead of `int`, `unsigned int`
- `enum class` for type-safe enumerations
- `constexpr` instead of `#define` for constants
- Stack-only allocation (no `new`/`delete`)
- `util::` library replaces STL (no `std::vector`, `std::string`)

**Known deviations** (acceptable):
- Some use of C-style casts for hardware register access
- Platform-specific `#ifdef` blocks for AVR vs. x86
- `PROGMEM` requires compiler-specific extensions

When suggesting improvements, maintain these project patterns.
