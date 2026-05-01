---
description: "Write or revise an arc42 Markdown document for a module using extracted repository evidence."
name: "Arc42 Write"
agent: "Arc42 Writer"
argument-hint: "Target module path, e.g. Src/Prj/App/Signal"
---

Write or update a high-quality arc42 document for the provided module.

## Steps

1. Use existing context docs if present under `docs/arc42/_context/`.
2. Build `docs/arc42/<module>.md` with full arc42 structure.
3. Add concrete runtime and deployment details where supported.
4. Add "Known Gaps" and unresolved assumptions.

If context is missing, run a quick extraction first before writing.
