---
description: "Apply a single MISRA C++ violation fix to a source file, verify the build compiles, and mark the violation as resolved in the analysis log."
name: "MISRA Fix"
agent: "Refactoring Specialist"
argument-hint: "File path and violation to fix, e.g. Src/Prj/App/Signal/DccDecoder.cpp: [Lines 113,115] size_t → uint16"
---

Apply exactly one MISRA violation fix from the user's message. The user will supply:
- The **source file** to fix
- The **violation** to address (copy-pasted from the analysis report, or described in their own words)

If either is missing, ask before proceeding.

## Steps

### 1. Load existing analysis

Check `.github/analysis/misra/<SourceRelPath>.md` for the most recent analysis section.
- `Src/Prj/App/Signal/DccDecoder.cpp` → `.github/analysis/misra/Signal/DccDecoder.md`
- `Src/Gen/Dcc/Decoder.cpp` → `.github/analysis/misra/Dcc/Decoder.md`

Read the violation description carefully. If the analysis file does not exist, ask if the user wants to run `/misra-review` first.

### 2. Read the source file

Read the full source file. Understand the context around the violation before touching anything.

### 3. Show the proposed change

Before making any edit, display the exact before/after diff to the user:

```
File: <path>

BEFORE (line XX):
  <original code>

AFTER:
  <fixed code>

Reason: <one sentence why this is compliant>
```

Ask: **"Proceed with this fix? (yes / no / modify)"**

Stop here and wait for the user's answer. Do not apply the edit until the user confirms.

### 4. Apply the fix

Apply only the single confirmed change. Do not fix other violations in the same pass, even if you notice them.

### 5. Verify the build

Run a Windows build to confirm the change compiles:

```
Build/build.bat <project> win32 gcc win none all
```

Derive `<project>` from the source file path:
- `Src/Prj/App/Signal/` → `App/Signal`
- `Src/Gen/Dcc/` → build from a unit test or the Signal app if no standalone build exists

If the build **fails**: revert the change, explain what went wrong, and stop. Do not attempt a second fix.

If the build **succeeds**: proceed to step 6.

### 6. Update the analysis log

In `.github/analysis/misra/<SourceRelPath>.md`, mark the violation as resolved by replacing the bullet with:

```
- ~~[Line XX] Rule X-X-X: <original text>~~ **Fixed YYYY-MM-DD**
```

Also update the violation counts in the summary line for the latest dated section.

### 7. Report to the user

Print:
- What was changed (file, line, before/after)
- Build result
- Updated violation count for the file
- The next highest-priority open violation from the report (as a suggestion for the next `/misra-fix` invocation)

## Constraints

- Fix **one violation per session** only.
- Do not fix other issues you spot in passing — note them at most as a reminder at the end.
- Do not change function signatures, public API, or observable behavior.
- Do not add new Doxygen, comments, or reformatting unrelated to the fix.
- If the fix requires changes to a header file (e.g. type change in a function signature), update the header too — but count that as part of the same single fix.
