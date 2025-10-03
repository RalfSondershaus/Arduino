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
