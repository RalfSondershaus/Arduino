---
description: "Generate or update Doxygen file header comments for C/C++ files following the project's embedded coding standards"
agent: "agent"
argument-hint: "Update file header"
tools: ['changes', 'edit/editFiles', 'search/codebase']
---

# Generate/Update Doxygen File Header

Update the Doxygen file header comment in the active C/C++ file to follow the project's embedded coding standards.

## Required Header Format

Every C/C++ source file (.h, .cpp) must start with a Doxygen file header comment containing:

```cpp
/**
 * @file <filename>
 *
 * @author <author name>
 *
 * @brief <concise description of file purpose>
 *
 * @copyright Copyright <year> <author>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
```

## Instructions

### Analysis Phase
1. Identify the current file being edited
2. Check if a Doxygen file header already exists
3. Analyze the file content to determine its purpose:
   - For classes: What is the class's responsibility?
   - For functions: What is the primary functionality?
   - For utilities: What utilities are provided?
4. Match existing code style and conventions

### Generation Rules

- **@file**: Use the actual filename with extension (e.g., `Blinker.h`, `Signal.cpp`)
- **@author**: Default to "Ralf Sondershaus" unless specified otherwise
- **@brief**: Write a clear, concise purpose statement:
  - Start with a verb for implementation files (e.g., "Implements...", "Defines...", "Provides...")
  - Use noun phrases for declaration files (e.g., "Signal processing component", "ADC driver interface")
  - Keep it under 80 characters
  - Be specific but succinct
- **@copyright**: Use format "Copyright YYYY Author Name" where YYYY is the current year (2026) for new files, or keep existing year for updates
- **SPDX-License-Identifier**: Always use "Apache-2.0"

### Update Behavior

**General Guidelines:**
- Do not alter any existing code or comments outside the header

**If header exists:**
- Preserve the existing copyright year and author
- Update @file if filename changed
- Improve @brief if it's unclear, generic, or doesn't match the actual code
- Maintain exact formatting (spacing, alignment)

**If header missing:**
- Create complete header at the very top of the file (before any `#ifndef` or includes)
- Use current year (2026) for copyright
- Generate appropriate @brief based on file analysis

### Quality Checks

- Ensure asterisks align vertically
- Maintain blank line after each tag (except between @file and @author)
- Confirm closing `*/` is on its own line
- Verify @brief is accurate and descriptive (not generic like "Header file" or "Implementation file")

## Examples

### Class Header File (.h)
```cpp
/**
 * @file SignalController.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Controls model railroad signal aspects and LED output timing
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */
```

### Implementation File (.cpp)
```cpp
/**
 * @file adc_driver.cpp
 *
 * @author Ralf Sondershaus
 *
 * @brief Implements ADC initialization and channel reading for Arduino platform
 *
 * @copyright Copyright 2026 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */
```

### Utility/Generic File
```cpp
/**
 * @file basic_string.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Fixed-capacity string container for embedded systems without dynamic allocation
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */
```

## Project Context

This project follows embedded C++ coding standards:
- Focus on what the file does (purpose/interface) not implementation details
- Implementation details belong in code comments within the .cpp file
- Headers document the public API and class responsibilities
- Follow MISRA C++ guidelines for safety-critical embedded systems
- No dynamic allocation, no exceptions, no RTTI

## Error Handling

- If file type is unsupported (not .h, .hpp, .cpp, .c), notify user
- If @brief cannot be determined from code analysis, suggest generic but accurate description based on filename
- If existing header has non-standard format, preserve custom elements while standardizing required fields
