# COM Layer Architecture (arc42)

> **Module path**: `Src/Gen/Com`  
> **Namespace**: `com::`  
> **Status**: active – generic, reusable

---

## 1. Introduction and Goals

`Src/Gen/Com` implements a lightweight serial ASCII communication stack for Arduino and host (Windows/Linux) targets. Its primary goals are:

- Receive newline-terminated ASCII telegrams from a serial port and dispatch them to a command handler.
- Provide a fixed generic command set (`SET_CV`, `GET_CV`, `MON_LIST`, `MON_START`, `MON_STOP`).
- Support project-specific command extensions without modifying the generic layer.
- Stay fully deterministic: no heap, no exceptions, no STL, fixed-capacity buffers.
- Run on both AVR (ATmega2560/328P) and x86 (Windows/Linux) via HAL abstraction.

---

## 2. Constraints

| Constraint | Source |
|---|---|
| No dynamic memory allocation | Embedded policy; `util::basic_string` used everywhere |
| No exceptions, no RTTI | Compiler flags (`-fno-exceptions`, `-fno-rtti`) |
| No virtual destructor | AVR-GCC: `operator delete` not available; objects never heap-allocated |
| Max telegram length 64 chars | `SerAsciiTP::kMaxLenTelegram = 64U` |
| Single observer per subject | `Subject` holds one `util::ptr<Observer>` |
| `cycle()` must be called periodically | Drives byte reception; missed calls risk serial buffer overflow |
| Fixed-width types only | `uint8`, `sint8`, `size_t` — no `int` widening |
| HAL abstraction mandatory | `hal::serial` must be used; `Arduino.h` never included in `Src/Gen` |

---

## 3. Context and Scope

**In scope** (`Src/Gen/Com`):

- All classes in the `com::` namespace: `Observer`, `Subject`, `SerComDrv`, `SerAsciiTP`, `AsciiCom`, `IfcAsciiCommandHandler`, `ComR`.
- Generic command dispatch and RTE port monitoring output.
- The extension interface (`IfcAsciiCommandHandler`) but not its project-specific implementations.

**Out of scope** (project layer):

- `Src/Prj/App/Signal/Com/` — Signal-specific command handler, `ComR_Prj`, and duplicated project-side headers.
- `hal::serial` implementation (platform-specific, lives in `Src/Gen/Hal/`).
- RTE and calibration (CV read/write performed by `AsciiCom` via `rte::get_cv()` / `rte::set_cv()`).

**External actors**:

| Actor | Direction | Interface |
|---|---|---|
| Serial terminal (user) | → COM | Bytes via UART (`hal::serial`) |
| Project command handler | ← COM | `IfcAsciiCommandHandler::process_command()` |
| RTE (CV access, monitoring) | ↔ COM | `rte::get_cv()`, `rte::set_cv()`, `rte::getPortData()` |
| Arduino `Serial` / host stub | → COM | `hal::serial` abstraction |

---

## 4. Solution Strategy

The stack is decomposed into three strictly ordered layers plus two base-class abstractions:

1. **Observer/Subject pattern** (`Observer.h`, `Subject.h`) — decouples the transport layer from the command parser without virtual dispatch overhead beyond `update()`.
2. **HAL adapter** (`SerComDrv`) — one-liner wrappers over `hal::serial::*`; swappable for stubs in unit tests.
3. **Transport layer** (`SerAsciiTP`) — byte accumulation into fixed-capacity strings; calls `Subject::notify()` on telegram completion.
4. **Command layer** (`AsciiCom`) — token-level dispatch; delegates unknown tokens to `IfcAsciiCommandHandler`.
5. **Composition root** (`ComR`) — owns all layer objects by value; wires them at `init()` time.

The extension point (`IfcAsciiCommandHandler`) partitions the return-code space so that `AsciiCom` owns the "OK" and "ERR: Invalid command" strings and project handlers own only project-specific error strings — eliminating duplicate string storage.

---

## 5. Building Block View

```
┌────────────────────────────────────────────────────┐
│                      ComR                          │
│  ┌──────────────┐  ┌───────────────┐  ┌─────────┐ │
│  │  SerComDrv   │  │  SerAsciiTP   │  │AsciiCom │ │
│  │  (HAL wrap)  │→ │  (transport)  │→ │(command)│ │
│  └──────────────┘  └───────────────┘  └────┬────┘ │
└───────────────────────────────────────────│────────┘
                                             │ (optional)
                                   ┌─────────▼──────────────┐
                                   │ IfcAsciiCommandHandler  │
                                   │  (project extension)    │
                                   └────────────────────────┘
```

| Block | Responsibility | Key types |
|---|---|---|
| `Observer` | Abstract base; defines `update()` | pure virtual |
| `Subject` | Holds one observer; calls `notify()` | `util::ptr<Observer>` |
| `SerComDrv` | Wraps `hal::serial` read/write/begin | `size_type`, `base_type` |
| `SerAsciiTP` | Byte → telegram assembly; overflow guard | `string_type` (64-char), `bOverflow` |
| `AsciiCom` | Token dispatch; generic CV and monitor commands | `string_type`, `util::ptr<IfcAsciiCommandHandler>` |
| `IfcAsciiCommandHandler` | Extension interface for project commands | `ret_type` (sint8), `kIfcOK/kIfcInvCmd/kIfcProjectBase` |
| `ComR` | Ownership + wiring of all layers | by-value members |

---

## 6. Runtime View

### 6.1 Nominal telegram receive/respond cycle

```
[Serial HW]  [SerComDrv]  [SerAsciiTP]       [Subject]    [AsciiCom]       [hal::serial]
     │             │             │                 │             │                │
     │  bytes avail│             │                 │             │                │
     │─────────────▶ available() │                 │             │                │
     │             │──read()────▶│                 │             │                │
     │             │  loop       │                 │             │                │
     │             │  until \n   │                 │             │                │
     │             │             │──notify()───────▶             │                │
     │             │             │                 │──update()───▶                │
     │             │             │                 │             │ process()      │
     │             │             │                 │             │ assemble resp  │
     │             │             │◀─transmitTelegram(response)───│                │
     │             │             │─────────────────────────────────write()+\r\n──▶│
```

### 6.2 Return-code routing in `AsciiCom::process()`

```
process_command() returns
  kIfcOK (0)          → prepend "OK" from AsciiCom's own table
  kIfcInvCmd (1)      → prepend "ERR: Invalid command" from AsciiCom's own table
  >= kIfcProjectBase  → call get_error_string() on project handler; use that prefix
```

### 6.3 Overflow path

When `telegram_rawdata.size() == kMaxLenTelegram` before a control character arrives:
- `bOverflow = true`; excess bytes discarded silently.
- On the next control character: `bOverflow` cleared, `telegram_rawdata` cleared, `notify()` **not** called.

---

## 7. Deployment View

| Target | `hal::serial` backend | Notes |
|---|---|---|
| Arduino Mega (ATmega2560) | `Serial` (64-byte HW buffer) | `cycle()` must run ≤ ~1 ms to avoid overrun |
| Arduino Nano (ATmega328P) | `Serial` (64-byte HW buffer) | Same timing constraint |
| Windows/Linux (x86/x64) | `Bsw/Win` stub | Used in unit tests and desktop simulation |

`ComR` is instantiated once per application (e.g. as a static member of the main module). On AVR it is typically stack-allocated at program scope.

---

## 8. Crosscutting Concepts

### Memory discipline
All strings are `util::basic_string<64, char>` — fixed-capacity, stack-allocated, no `std::string`. The response buffer `telegram_response` is a member of `AsciiCom`; no heap is ever touched.

### Observer/Subject pattern
A deliberate single-observer limitation keeps the pattern overhead minimal. The `util::ptr<Observer>` is a thin non-owning pointer wrapper (not `std::unique_ptr`).

### PROGMEM / ROM storage
Response literal strings in `AsciiCom.cpp` are marked `ROM_CONST_VAR` (mapped to `PROGMEM` on AVR). Access uses `ROM_READ_STRING()` / `ROM_READ_PTR()` to copy into a stack buffer before use, avoiding direct flash reads that crash on AVR.

### Extension interface return codes
`IfcAsciiCommandHandler::ret_type` (= `sint8`) partitions the return-code space at compile time:
- `0` = OK, `1` = invalid command (owned by `AsciiCom`).
- `>= 2` = project-specific (owned by the project handler).
The helper `is_project_specific_error()` makes the branch condition readable.

### No virtual destructor
All COM classes avoid virtual destructors. On AVR, linking a virtual destructor pulls in `operator delete`, which is not provided. Objects are static-duration or stack-allocated and never explicitly destroyed.

---

## 9. Architecture Decisions

### ADR-COM-01: Single-observer Subject

**Decision**: `Subject` holds exactly one `util::ptr<Observer>`.  
**Rationale**: Only one command parser (`AsciiCom`) ever listens to `SerAsciiTP`. A single pointer costs 2 bytes (AVR) vs. a list overhead. No current or planned use case requires multiple observers.  
**Trade-off**: Extending to multiple observers would require adding a small fixed-capacity array.

### ADR-COM-02: Return-code space partition for command extensions

**Decision**: `IfcAsciiCommandHandler` defines `kIfcOK=0`, `kIfcInvCmd=1`, `kIfcProjectBase=2`. Generic response strings live exclusively in `AsciiCom`; project error strings live exclusively in the project handler.  
**Rationale**: Eliminates duplicate storage of "OK" / "ERR: Invalid command" in both `AsciiCom` and project handlers.  
**Consequence**: Project codes must not collide with 0 and 1; `is_project_specific_error()` enforces this at the call site.

### ADR-COM-03: ComR as composition root by-value

**Decision**: `ComR` owns `SerComDrv`, `SerAsciiTP`, and `AsciiCom` by value (not pointer/reference).  
**Rationale**: No dynamic allocation. All three objects have identical lifetime (program scope). Wiring via pointers/references at `init()` time is safe because all members outlive the wiring call.

---

## 10. Quality Requirements

| Attribute | Requirement | Verification |
|---|---|---|
| Determinism | No heap; fixed-size buffers; no unbounded loops in `cycle()` | Code inspection; no `new`/`delete` |
| Timing | `cycle()` must complete within the serial overrun window (~1 ms at 115200 baud) | Manual timing analysis |
| Robustness | Telegrams > 64 chars are discarded without state corruption | `bOverflow` path in `SerAsciiTP::cycle()` |
| Portability | Must build on AVR-GCC and GCC (x86) without conditional compilation in `Src/Gen/Com` | Build system: `avr_gcc` and `gcc win32` targets |
| Testability | `SerComDrv` wraps HAL; HAL provides stubs → `AsciiCom`/`SerAsciiTP` testable on host | Unit tests under `Src/Prj/UnitTest/` |

---

## 11. Risks and Technical Debt

| Item | Severity | Description |
|---|---|---|
| Single observer limit | Low | Extending to N observers requires replacing `util::ptr<Observer>` with a small fixed array; straightforward but currently unneeded. |
| `ComR.h` legacy UDP/CAN-TP comments | Low | `ComR.h` contains large commented-out protocol notes (UDP, IPv4, CAN-TP, SerTP) that are unrelated to the current implementation. They should be moved to a design-notes document or removed. |
| `bOverflow` is not reset on `init()` | Low | `SerAsciiTP::init()` is a no-op; if `init()` is called mid-session `bOverflow` retains its state. In practice `init()` is only called once at startup. |
| Project-side header copies | Medium | `Src/Prj/App/Signal/Com/` contains header copies of `Observer.h`, `Subject.h`, `SerAsciiTP.h`, etc. These may diverge from `Src/Gen/Com/`. The project copies should include the generic headers instead of maintaining separate copies. |

---

## 12. Glossary

| Term | Definition |
|---|---|
| Telegram | A complete ASCII string received from the serial port, terminated by a control character (e.g. `\n`), max 64 chars. |
| CV | Configuration Variable; EEPROM-backed value following DCC CV convention. |
| Observer | Abstract base class in `com::` whose `update()` is called by a `Subject` when an event occurs. |
| Subject | Abstract base class in `com::` that holds an observer reference and calls `notify()`. |
| `ret_type` | `sint8` return type of `IfcAsciiCommandHandler::process_command()`; partitioned into generic codes (0–1) and project codes (≥ 2). |
| HAL | Hardware Abstraction Layer; `hal::serial` wraps Arduino `Serial` on AVR and a stub on x86. |
| `ROM_CONST_VAR` | Macro that expands to `PROGMEM` on AVR and to nothing on x86; marks constants for flash storage. |
| ComR | Composition root that owns `SerComDrv`, `SerAsciiTP`, and `AsciiCom` by value and wires them at `init()`. |

---

## Traceability

| Claim | Source file(s) reviewed |
|---|---|
| `kMaxLenTelegram = 64` | [Src/Gen/Com/SerAsciiTP.h](../../Src/Gen/Com/SerAsciiTP.h) |
| Single-observer `Subject` | [Src/Gen/Com/Subject.h](../../Src/Gen/Com/Subject.h) |
| Return-code partition | [Src/Gen/Com/IfcAsciiCommandHandler.h](../../Src/Gen/Com/IfcAsciiCommandHandler.h) |
| Generic command set | [Src/Gen/Com/AsciiCom.h](../../Src/Gen/Com/AsciiCom.h) |
| ComR wiring sequence | [Src/Gen/Com/ComR.h](../../Src/Gen/Com/ComR.h), [Src/Gen/Com/ComR.cpp](../../Src/Gen/Com/ComR.cpp) |
| Overflow path | [Src/Gen/Com/SerAsciiTP.cpp](../../Src/Gen/Com/SerAsciiTP.cpp) |
| PROGMEM access pattern | [Src/Gen/Com/AsciiCom.cpp](../../Src/Gen/Com/AsciiCom.cpp) |
| No virtual destructor rationale | [Src/Gen/Com/IfcAsciiCommandHandler.h](../../Src/Gen/Com/IfcAsciiCommandHandler.h), [Src/Gen/Com/Observer.h](../../Src/Gen/Com/Observer.h) |

---

## Open Points

- **Section 7**: Exact AVR serial buffer size at 115200 baud and maximum tolerable `cycle()` latency have not been formally measured — marked as a quality risk.
- **Section 11**: Confirm whether the project-side header copies (`Src/Prj/App/Signal/Com/Observer.h` etc.) are intentional overrides or accidental divergence.
- **Section 6.1**: `AsciiCom::process()` internal dispatch detail (error-code routing) should be verified against the current `AsciiCom.cpp` implementation for full accuracy.
