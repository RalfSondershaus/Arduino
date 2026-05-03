---
description: "Improve all Doxygen comments in a C/C++ file: file header, classes, functions, members. Checks consistency between comments and code."
agent: "agent"
argument-hint: "Improve Doxygen comments"
tools: ['edit/editFiles', 'search/codebase']
---

# Improve Doxygen Comments

Improve **all** Doxygen comments in the active C/C++ file.
Apply the rules below strictly, using the [Style Reference](#style-reference) as the authoritative example.
Do **not** change any code — only comments.

---

## Phase 1 — Consistency Check

Before writing any comments, scan the file for inconsistencies between existing comments and the actual code. Flag each one (as an inline `// TODO(doc): ...` comment next to the affected Doxygen block) if it cannot be resolved automatically:

- `@param` names that do not match the actual parameter names in the signature
- `@param` or `@return` descriptions that contradict the implementation logic
- `@tparam` entries missing for template parameters that exist in the signature
- `@brief` that describes a different behaviour than what the code implements
- Member variable `///<` comments that are stale (e.g., describe a field that was renamed or repurposed)
- Return value descriptions that do not cover all `return` statements
- `@see` / `@ref` that point to symbols that no longer exist

Resolve automatically where the fix is unambiguous. Leave a `// TODO(doc):` marker where human judgement is needed.

---

## Phase 2 — File Header

Every `.h` and `.cpp` file must start with exactly this structure (before any `#ifndef` or `#include`):

```cpp
/**
 * @file <filename-with-extension>
 *
 * @author Ralf Sondershaus
 *
 * @brief <concise purpose, ≤ 80 chars>
 *
 * @copyright Copyright <year> Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */
```

Rules:
- **@file**: filename only, no path
- **@brief**: start with a noun phrase for `.h`, a verb phrase for `.cpp` (e.g., "Implements...", "Fixed-capacity...")
- **@copyright**: preserve existing year; use 2026 for new files
- Blank line after every tag except between `@file` and `@author`
- Asterisks align vertically; closing `*/` on its own line

---

## Phase 3 — Class / Struct / Enum Documentation

Every class, struct, and enum needs a `/** ... */` block immediately before it:

```cpp
/**
 * @brief <one-sentence purpose of the class>.
 *
 * <Optional extended description: what it does, when to use it,
 * key invariants, algorithm summary, or usage constraints.>
 *
 * Example usage:
 * @code
 * MyClass obj;
 * obj.do_something(42);
 * @endcode
 *
 * @tparam T Description of template parameter T.
 * @tparam N Description of template parameter N.
 *
 * @see RelatedClass
 */
```

Rules:
- `@tparam` is **mandatory** for every template parameter — include valid range or meaning where relevant
- Include `@code` / `@endcode` example for non-trivial classes
- Document key constraints (e.g., "No dynamic allocation", "Must be called cyclically")
- List important invariants or state transitions if applicable (see `classifier` in Style Reference)

---

## Phase 4 — Member Variable Documentation

Use inline `///<` on the same line for simple members, or `/// ` above for longer explanations:

```cpp
class_type current_class;         ///< Current discrete class (0..NrClasses-1), kInvalidIndex if no match
MilliTimer debounce_timer_ms;     ///< Debounce timer; expires after get_debounce_time_ms()
uint8 classifier_type;            ///< Selects which calibration limit set to use
```

Rules:
- Every non-trivial member variable needs a `///<` comment
- State valid ranges or units in brackets: `///< [ms] cycle time`, `///< 0..1023`
- Avoid redundant phrases like "stores the value of..." — say what the value *means*
- Type aliases (`using`, `typedef`) need a brief `///<` or `///` comment when their purpose is not obvious

---

## Phase 5 — Function Documentation

Every non-trivial function (i.e., not a one-liner getter with an obvious name) needs a `/** ... */` block:

```cpp
/**
 * @brief <imperative verb phrase — what the function does, ≤ 80 chars>.
 *
 * <Optional extended description: preconditions, algorithm notes, side effects,
 * or important behavioural details that are not obvious from the signature.>
 *
 * @param[in]  param_name Description including valid range if applicable.
 * @param[out] result     Description of what is written.
 * @param[in,out] buf     Description of read/write parameter.
 * @return <description of return value and all meaningful return states>
 *
 * @note <Important constraint or caveat — e.g., "Must be called cyclically".>
 * @see  <Related function or class>
 */
```

Rules for `@param`:
- Use `[in]`, `[out]`, `[in,out]` direction tags
- Include value range where meaningful: `(10-bit ADC value: 0...1023)`, `[ms]`
- Name must exactly match the parameter in the signature

Rules for `@return`:
- Cover **every** `return` statement: describe what each return value means
- For `bool`: document both `true` and `false` cases explicitly
- For enums/error codes: list each relevant enumerator

Rules for `@note`:
- Use for ISR-safety requirements, cyclic-call requirements, or PROGMEM access constraints

Omit the block for:
- Trivial one-liner getters/setters where name + type are self-documenting (`///<` on the declaration is sufficient)

---

## Style Reference

`Classifier.h` is the authoritative style example for this project. Key patterns to replicate:

**Class-level algorithm description:**
```cpp
/**
 * @brief Template class for classifying analog input values into discrete classes.
 *
 * - Class 0: anLow[0] <= M <= anHigh[0] for time debounce_timer_ms
 * - Class 1: anLow[1] <= M <= anHigh[1] for time debounce_timer_ms
 * ...
 *
 * @tparam NrClasses Number of discrete classes to classify input values into.
 * @see classifier_cal
 */
```

**Value range in @param:**
```cpp
 * @param val Raw input value to be classified (10-bit ADC value: 0...1023)
```

**bool return — both cases explicit:**
```cpp
 * @return true: continue to next list element
 * @return false: stop, end of list
```

**Inline member comment:**
```cpp
class_type current_class; ///< Current discrete class, kInvalidIndex if no match
```

**Type alias comment:**
```cpp
using input_type = uint16; /**< measurements are 10-bit unsigned AD values: 0 ... 1023 */
```

---

## Project Context

- Embedded C++, Arduino (AVR Mega/Nano) and Windows/Linux host builds
- No dynamic allocation, no exceptions, no RTTI
- MISRA C++ safety-critical coding rules apply
- Platform macros: `ROM_CONST_VAR` (PROGMEM on AVR, empty on x86), `ROM_READ_PTR()`, `ROM_READ_STRING()`
- Fixed-width types: `uint8`, `uint16`, `uint32`, `sint8`, `sint16`, `boolean`
- Generic utilities in `util::` namespace (not STL)
- Layer prefixes: `hal::`, `rte::`, `cal::`, `dcc::`, `signal::`, `com::`
- `.h` files document the *what* (public API); `.cpp` files document the *how/why*

---

## Output Requirements

1. Apply all changes directly to the file — do not produce a diff or description only
2. Preserve all existing code exactly (indentation, line endings, logic)
3. Preserve comments that are already correct and complete
4. If a function body is needed to resolve a consistency issue, read it before writing the comment
5. Do not add `@author` to individual functions — only to the file header
