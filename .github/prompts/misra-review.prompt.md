---
description: "Run MISRA C++ compliance analysis on a specific file or module. Produces a structured violation report and appends it to .github/analysis/misra-analysis.md."
name: "MISRA Review File"
agent: "MISRA Reviewer"
argument-hint: "Path to the file or folder to analyze, e.g. Src/Gen/Dcc/BitExtractor.h"
---

Perform a full MISRA C++ compliance analysis on the file or folder the user specified in their message (the path provided after the slash command). If no path was provided, ask the user to specify one before proceeding.

## Steps

1. Read the target file(s) completely.
2. For each file, produce a structured report in this format:

```
## File: <relative/path/to/file>

### CRITICAL
- [Line XX] Rule X-X-X: <description> — `<offending code>`. Fix: <compliant alternative>.

### HIGH
- [Line XX] <description>

### MEDIUM
- [Line XX] <description>

### COMPLIANT
- <what is already correct>
```

3. If the target is a folder, analyze each `.h` and `.cpp` file and produce one section per file.
4. Save the complete report to `.github/analysis/misra/<SourceRelPath>.md`:
   - Map the source file path to the analysis path by replacing `Src/` prefix layers with a shorter form, e.g.:
     - `Src/Prj/App/Signal/DccDecoder.cpp` → `.github/analysis/misra/Signal/DccDecoder.md`
     - `Src/Gen/Dcc/Decoder.cpp` → `.github/analysis/misra/Dcc/Decoder.md`
   - If the file already exists, prepend a new dated section (`## YYYY-MM-DD`) above the previous ones.
   - Create the file and any intermediate directories if they don't exist.
   - Also add or update a row in the index table in `.github/analysis/misra-analysis.md`.
5. Print a summary: total files analyzed, violation counts by severity, and the top 3 critical issues found.
