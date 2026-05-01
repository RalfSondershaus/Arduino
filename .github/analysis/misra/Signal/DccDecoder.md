# MISRA Analysis: Src/Prj/App/Signal/DccDecoder.cpp

> One section per analysis run. Latest run first.
> Maintained by the `misra-review` agent.

---

## 2026-04-30

**Summary**: 0 Critical, 4 High, 4 Medium violations.

### CRITICAL

None.

### HIGH

- ~~[Lines 113, 115, 144] Rule 3-9-2 — Non-fixed-width type `size_t` for `idx` and `pos`. Width is 16-bit on AVR, 32/64-bit on x86 — masks overflow differences between target and test builds.~~ **Fixed 2026-04-30**

- **[Lines 144–147]** Inconsistency / misleading debug output — `extended_packet_received` lacks the
  `if (pkt_address >= get_first_output_address())` guard present in `basic_packet_received` (line 107).
  Although `boundaryCheck` prevents any out-of-bounds write, if `pkt_address < first_output_address`
  the wrapped `pos` value is printed to serial (line 147) before the check, producing garbage debug output.
  **Fix**: Add the same entry guard as in `basic_packet_received`.

- **[Lines 121, 123, 148]** Rule 3-9-2 — `static_cast<int>(pos)` and `static_cast<int>(cmd)`.
  `int` is non-fixed-width. **Fix**: `static_cast<uint16>(pos)`, `static_cast<uint16>(cmd)`.

- ~~[Lines 176, 179] Rule 2-10-1 — Typographic ambiguity: `packet_type::packet_type::BasicAccessory`. Class alias `packet_type` and nested enum `packet_type` share the same identifier.~~ **Fixed 2026-04-30**

- **[Line 117]** Rule 5-0-3 — Mixed-width arithmetic: `2U * idx + pkt.ba_get_output_direction()` mixes `size_t`/`uint16` with `uint8`, result narrowed to `uint8` via explicit cast. Intermediate widening not guarded.
  **Fix**: Use `static_cast<uint8>(2U) * static_cast<uint8>(idx)` or validate that `idx` fits `uint8` range.

### MEDIUM

- **[Line 50]** Rule 3-7-2 — Function-scoped static `static uint8 lastWrite = LOW`: initialization
  order is implementation-defined. Also uses `#define LOW` instead of a `constexpr`-typed constant.
  **Fix**: `static uint8 lastWrite = static_cast<uint8>(0U);`

- **[Line 70]** Rule 3-7-2 — Function-scoped static `static util::MilliTimer LedTimer`:
  initialization order implementation-defined. Acceptable only on single-threaded AVR; document it.

- **[Lines 50, 54, 56, 58]** Rule 8-4-4 (advisory) — `LOW`/`HIGH` macros from `Hal/Gpio.h` used
  as value constants. Prefer typed `constexpr` equivalents if HAL exposes them.

### COMPLIANT

- No `Arduino.h` included directly — hardware through `Hal/` ✓
- No dynamic memory (`new`/`delete`) ✓
- No STL usage ✓
- No `goto`, no recursion ✓
- `static_cast<>` used instead of C-style casts ✓
- Fixed-width types `uint8`, `uint16` used predominantly ✓
- `switch` has `default: break;` ✓
- Doxygen on all public/protected methods ✓
- Namespaces `signal::` and `signal_cal::` consistent ✓
- `const` on all non-mutating locals ✓
- `kPascalCase` constants in header ✓
