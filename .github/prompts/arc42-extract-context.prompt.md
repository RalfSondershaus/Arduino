---
description: "Extract architecture facts from code/build files for a target module and save a context brief."
name: "Arc42 Context Extract"
agent: "Code-to-Context Extractor"
argument-hint: "Target module path, e.g. Src/Gen/Dcc or Src/Prj/App/Signal"
---

Extract implementation-grounded architecture context for the provided module.

## Steps

1. Read module headers/sources plus related build files.
2. Capture interfaces, runtime flow, configuration points, and constraints.
3. Save findings to `docs/arc42/_context/<module>-context.md`.
4. Include a final list of evidence file paths.

If the user supplied a folder, include all relevant `.h`, `.cpp`, and makefile files.
