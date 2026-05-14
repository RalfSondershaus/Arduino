# Embedded C++ Coding Style Guide

A practical style guide for safety-critical embedded systems, inspired by Linux Kernel conventions, 
Quantum Leaps, and MISRA/AUTOSAR standards.

Coding styles are like traffic rules: they don’t limit creativity, they enable smooth flow. 

## Rule Levels

This coding style defines two rule levels:

- **Mandatory (SHALL):** Required for all production code. Violations must be fixed or formally
  documented as a deviation. Keyword allowed: **SHALL**
- **Advisory (SHOULD):** Non-mandatory guidance. Deviations are allowed with rationale. 
  Keywords allowed: **SHOULD**, **AVOID**, **PREFER**, **MAY**

The key words SHALL, SHOULD, and MAY are to be interpreted as described in RFC 2119 / RFC 8174.

---

## File Names

- SHOULD use `snake_case`
- Extensions SHALL be: `.cpp` for implementation, `.h` for headers
- AVOID uppercase letters and special characters

**Examples:**
- `motor_control.cpp`
- `adc_driver.h`

---

## Variable Names

- SHALL use `snake_case` for all variables
- AVOID type prefixes like `uc_`, `un_`
- Type information SHOULD come from the declaration, not the name
- AVOID tricky or cryptic expressions
- Readability over brevity: names SHOULD be clear and descriptive

| Instead of | Better alternatives     |
|------------|-------------------------|
| `uc_data`  | `data` or `byte_data`   |
| `un_count` | `count` or `item_count` |
| `ul_index` | `index` or `long_index` |
| `nr_elems` | `num_elems` or `elems_count` |

**Examples:**
```cpp
uint16 adc_value;
boolean motor_enabled;
float temperature_celsius;
```

## Function Names
- SHALL use `snake_case` with module prefixes
- Names SHOULD be descriptive and action-oriented
- AVOID uppercase letters and special characters
- AVOID overloaded functions with side effects

**Examples**:
```cpp
void adc_init();
uint16 adc_read_channel(uint8 channel);
void motor_enable();
void motor_disable();
```

## Type Names (Classes, Structs, Enums)
- SHALL use `PascalCase` for class, struct, and enum names in project specific files
- SHALL use `snake_case` for class, struct, and enum names in generic files (e.g. `util::`)
- PREFER `enum class` for type safety
- AVOID multiple inheritance and complex templates
- For global types, AVOID the _t suffix to prevent conflicts with POSIX
- PREFER the _type suffix for member types in templates or traits.

**Examples**:
```cpp
class MotorController;
struct AdcConfig;
enum class MotorState { eOff, eOn, eError };
namespace util 
{
    class array;
}
```

## Constants
- SHALL use `constexpr` or `static const` instead of `#define`
- SHALL use `kPascalCase` for `constexpr` or `static const`
- SHALL use `ALL_CAPS` for `#define`

**Examples:**
```cpp
static constexpr uint16 kMaxSpeed = 1200;
static constexpr float kTemperatureThreshold = 75.0f;
```

## Comments

### Doxygen

Doxygen comments SHALL stay consistent with the code. When a parameter, return value, or class 
invariant changes, update the comment in the same commit. You can also use a prompt such as
`doxygen-improve` for this but review the outcome.

Important tags

|Tag        | Meaning                   |
|-----------|-----------------------------|
| `@brief`  | Short description           |
| `@param`  | Parameter description. SHALL document units ([ms], [us]) and valid ranges (0...1023, 0..NrClasses-1) in @param descriptions. |
| `@return` | Return value. For `boolean` returns, it SHALL document both `true` and `false` explicitly. For `enum` returns, it SHALL list each relevant enumerator. |
| `@see`    | Reference to other elements |
| `@note`   | Additional notes            |
| `@todo`   | Tasks or pending items      |
| `@ref`    | Reference to other symbols  |
| `@tparam` | Template parameter          |
| `@code`, `@endcode` | Code snippets          |

`@tparam` SHALL be used for every template parameter and SHALL include the valid range or the role of the parameter.

`@code` / `@endcode` SHOULD be used to include short usage examples for non-trivial classes or functions.

#### Comment style variants

When a comment needs multiple lines, `/** ... */` blocks SHALL be used.

|Situation                                  | Style                       |
|-------------------------------------------|-----------------------------|
|Class, struct, function	                | `/** @brief ... */` block   |
|Single-line member variable (same line)    | `///<`                      |
|Multi-line member description (line above) | `/** ... */` block          |
|Type alias with non-obvious purpose        | `///<` inline               |

### Comments in the Header File (.h)

When appropriate:
- **Public API documentation**
   - e.g. function descriptions, parameters, return values
- **Class descriptions / purpose of the class**
   - What the class does and how it should be used
- **Constants, type definitions, enums**
   - Meaning, value range, constraints

### Comments in the Implementation File (.cpp)

When appropriate:
- **Algorithm explanation**
   - Why something is done a certain way, edge cases
- **Optimizations or workarounds**
   - e.g. hardware quirks, compiler-specific behavior
- **Temporary notes / TODOs**
   - e.g. // TODO(Ralf): Handle overflow case

### Best practice

| Type of comment          | Location              |
|--------------------------|-----------------------|
| What the function does   | Header file (.h)      |
| How it works             | Implementation (.cpp) |
| Why it works that way    | Implementation (.cpp) |

## Additional Rules and Recommendations
- SHALL not use dynamic memory allocation (`new`, `malloc`)
- SHALL not use RTTI.
- SHALL not use exceptions.
- SHOULD use `namespace` for logical grouping
- SHALL use module prefixes for clarity (`uart_`, `gpio_`, `adc_`)
- SHALL comment hardware-specific logic and register access
- Lines SHALL NOT exceed 100 characters
  - Exception: URLs, generated code, and unbreakable string literals MAY exceed 100 characters.
  - In these cases, line length SHOULD be minimized where possible.

### Bitfields (advisory)
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
- AVOID bitfields
- Use bit masks and shift operations instead
```cpp
constexpr uint32 kMaskA = 0xE0; // Bits 5–7
constexpr uint32 kMaskB = 0x1F; // Bits 0–4
constexpr uint32 kShiftA = 5; // Bits 5–7
constexpr uint32 kShiftB = 0; // Bits 0–4

uint32 value = (a & kMaskA) >> kShiftA;
```
This gives you full control over bit assignment — independent of the compiler.

## References
- [Linux Kernel Coding Style](https://kernel.org/doc/html/v4.10/process/coding-style.html)
- [Quantum Leaps Embedded C++ Style Guide](https://github.com/QuantumLeaps/embedded-coding-style)
- [MISRA C++ Guidelines](https://www.perforce.com/resources/qac/misra-c-cpp)
- [AUTOSAR C++14 Guidelines PDF](https://www.autosar.org/fileadmin/standards/R18-03_R1.4.0/AP/AUTOSAR_RS_CPP14Guidelines.pdf)

## Why these conventions?

- **Clarity**: Snake case separates words clearly — especially important for long variable names.
- **Tool compatibility**: Many static analysis tools expect specific naming patterns.
- **Avoiding Errors**: Styles help prevent subtle bugs — like confusing variable names or ambiguous 
logic. Naming conventions signal type, scope, or purpose.
- **Consistency = Readability**: When code follows a consistent style, it’s easier to read and understand — even if you didn’t 
write it. Think of it like punctuation in writing: without it, even brilliant ideas become a mess.
- **Team Collaboration**: In teams, coding styles prevent “style wars” and reduce friction during code reviews.
Everyone speaks the same “code dialect,” so the focus stays on logic, not formatting.
- **Maintainability Over Time**: Code lives longer than you think. Months or years later, someone (maybe you!) will revisit it. 
A clear style helps future developers understand intent without deciphering cryptic naming or 
structure.
