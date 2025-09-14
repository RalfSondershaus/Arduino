# Embedded C++ Coding Style Guide

A practical style guide for safety-critical embedded systems, inspired by Linux Kernel conventions, 
Quantum Leaps, and MISRA/AUTOSAR standards.

Coding styles are like traffic rules: they don’t limit creativity, they enable smooth flow. 

## Why these conventions? ##

1. Consistency = Readability
When code follows a consistent style, it’s easier to read and understand — even if you didn’t 
write it. Think of it like punctuation in writing: without it, even brilliant ideas become a mess.

2. Team Collaboration
In teams, coding styles prevent “style wars” and reduce friction during code reviews.
Everyone speaks the same “code dialect,” so the focus stays on logic, not formatting.

3. Tooling & Automation
Linters, formatters, and static analysis tools rely on predictable patterns. A consistent style 
makes it easier to catch bugs, enforce rules, and automate formatting.

4. Maintainability Over Time
Code lives longer than you think. Months or years later, someone (maybe you!) will revisit it. 
A clear style helps future developers understand intent without deciphering cryptic naming or 
structure.

5. Avoiding Errors
Styles help prevent subtle bugs — like confusing variable names or ambiguous logic. Naming 
conventions signal type, scope, or purpose.

---

## File Names

- Use `snake_case`
- Extensions: `.cpp` for implementation, `.h` for headers
- Avoid uppercase letters and special characters

**Examples:**
- `motor_control.cpp`
- `adc_driver.hpp`

---

## Variable Names

- Use `snake_case` for all variables
- Avoid type prefixes like `uc_`, `un_`
- Type information should come from the declaration, not the name
- Avoid tricky or cryptic expressions
- Readability over brevity: names should be clear and descriptive

| Instead of | Better alternatives     |
|------------|-------------------------|
| `uc_data`  | `data` or `byte_data`   |
| `un_count` | `count` or `item_count` |
| `ul_index` | `index` or `long_index` |
| `nr_elems` | `num_elems` or `elems_count` |

**Examples:**
```cpp
uint16_t adc_value;
bool motor_enabled;
float temperature_celsius;
```

## Function Names
- Use `snake_case` with module prefixes
- Names should be descriptive and action-oriented
- Avoid uppercase letters and special characters
- Avoid overloaded functions with side effects

**Examples**:
```cpp
void adc_init();
uint16_t adc_read_channel(uint8_t channel);
void motor_enable();
void motor_disable();
```

## Type Names (Classes, Structs, Enums)
- Use `PascalCase` for class, struct, and enum names in project specific files
- Use `snake_case` for class, struct, and enum names in generic files (e.g. `util::`)
- Prefer `enum class` for type safety
- Avoid multiple inheritance and complex templates
- For global types, it's better to avoid the _t suffix to prevent conflicts with POSIX
- Prefer the _type suffix for member types in templates or traits.

**Examples**:
```cpp
class MotorController;
struct AdcConfig;
enum class MotorState { eOff, eOn, eError };
```

## Constants
- Use `constexpr` or `static const` instead of `#define`
- Naming: `kPascalCase` or `ALL_CAPS`

**Examples:**
```cpp
static constexpr uint16_t kMaxSpeed = 1200;
static constexpr float kTemperatureThreshold = 75.0f;
```

## Comments

### Doxygen
Wichtige Tags
Tag	Bedeutung
@brief	Kurzbeschreibung
@param	Parameterbeschreibung
@return	Rückgabewert
@see	Verweis auf andere Elemente
@note	Zusätzliche Hinweise
@todo	Aufgaben oder offene Punkte
@ref	Referenz auf andere Symbole
### Comments in the Header File (.h)

When appropriate:
- Public API documentation 
   → e.g. function descriptions, parameters, return values → Ideal for Doxygen or other documentation tools
- Class descriptions / purpose of the class
   → What the class does and how it should be used
- Constants, type definitions, enums
   → Meaning, value range, constraints

### Comments in the Implementation File (.cpp)

When appropriate:
- Algorithm explanation 
   → Why something is done a certain way, edge cases
- Optimizations or workarounds 
   → e.g. hardware quirks, compiler-specific behavior
- Temporary notes / TODOs
   → e.g. // TODO(ralf): Handle overflow case

### Best practice

| Type of comment          | Location              |
|--------------------------|-----------------------|
| What the function does   | Header file (.h)      |
| How it works             | Implementation (.cpp) |
| Why it works that way    | Implementation (.cpp) |

## Additional Recommendations
- Avoid dynamic memory allocation (`new`, `malloc`)
- No RTTI, no exceptions
- Use `namespace` for logical grouping
- Use module prefixes for clarity (`uart_`, `gpio_`, `adc_`)
- Comment hardware-specific logic and register access

### Bitfields
The C/C++ standards do not specify whether bitfields are assigned starting from the Least 
Significant Bit (LSB) or the Most Significant Bit (MSB). That means:
- The bit order in a bitfield depends on the compiler and the target platform. 
- Even different versions of the same compiler may behave differently.
- There is no portable guarantee
```cpp
struct Flags {
    unsigned int a : 3;
    unsigned int b : 5;
};
```
Whether a occupies the lower or higher bits is not standardized. On some systems, a will be in 
the higher bits, on others in the lower bits.

If you need precise control over bit positions (e.g. for hardware registers or binary protocols), 
then:
- Avoid bitfields
- Use bit masks and shift operations instead
```cpp
constexpr uint32_t MASK_A = 0xE0; // Bits 5–7
constexpr uint32_t MASK_B = 0x1F; // Bits 0–4

uint32_t value = (a << 5) | b;
```
This gives you full control over bit assignment — independent of the compiler.

## References
- [Linux Kernel Coding Style](https://kernel.org/doc/html/v4.10/process/coding-style.html)
- [Quantum Leaps Embedded C++ Style Guide](https://github.com/QuantumLeaps/embedded-coding-style)
- [MISRA C++ Guidelines](https://www.perforce.com/resources/qac/misra-c-cpp)
- [AUTOSAR C++14 Guidelines PDF](https://www.autosar.org/fileadmin/standards/R18-03_R1.4.0/AP/AUTOSAR_RS_CPP14Guidelines.pdf)

## Why these conventions? ##

- Clarity: Snake case separates words clearly — especially important for long variable names.
- Tool compatibility: Many static analysis tools expect specific naming patterns.
- Avoiding name conflicts: For example, preventing overlap with macros or global symbols.
