---
description: "Create or restructure arc42 documentation layout for a target module/application."
name: "Arc42 Architect"
agent: "Documentation Architect"
argument-hint: "Target module path, e.g. Src/Prj/App/Signal"
---

Design or refine arc42 documentation for the module path provided by the user.

## Steps

1. Read existing docs and target source tree.
2. Create or update `docs/arc42/<module>.md` with full arc42 section scaffold.
3. Fill sections that can be evidenced from repository content.
4. Mark missing evidence as `TODO: evidence required`.
5. Add a short "Traceability" section mapping claims to source files.

If no module path is provided, ask for one before continuing.
